
#include <queue>
#include <iostream>
#include <fstream> 
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/function.hpp>

#include <boost/lexical_cast.hpp>


using namespace std;
using namespace boost::asio;

class Logger
{
	std::ofstream ofs;
public:
	Logger() = default;

	Logger(string str_) : ofs(str_)
	{
		ofs << "Recording started\n\n";
	};

	void log(string str, int i)
	{
		ofs << "[" << i << "] - " << str << "\n";
	}
};

using socket_ptr		= boost::shared_ptr<ip::tcp::socket>;
using io_service_ptr	= boost::shared_ptr<boost::asio::io_service>;
using work_ptr			= boost::shared_ptr<boost::asio::io_service::work>;


class Server_Http
{
	int							_nThread_Pool,
								_port;

	io_service					_service;

	vector<work_ptr>			_work;
	vector<boost::thread>		_thread_Pool;
	vector<io_service_ptr>		_io_service;

	shared_ptr<ip::tcp::acceptor> _acceptor;

	boost::shared_ptr <boost::thread> thread_;

public:

	Server_Http(int	port = 3000, int tp = boost::thread::hardware_concurrency() - 6) :
		_port(port), _nThread_Pool(tp)
	{};

	void start()
	{
	//	_acceptor = make_shared<ip::tcp::acceptor>(_service, ip::tcp::endpoint(ip::tcp::v4(), _port));
	//	listener(_service, _acceptor);
		thread_ = boost::make_shared<boost::thread>(boost::bind(&Server_Http::listener, this));
	}

	void stop()
	{
		_acceptor->close();
		_service.stop();
		thread_->join();
	}

	void listener()
	{
		_acceptor = make_shared<ip::tcp::acceptor>(_service, ip::tcp::endpoint(ip::tcp::v4(), _port));
		
		_work.reserve(_nThread_Pool);
		_io_service.reserve(_nThread_Pool);
		_thread_Pool.reserve(_nThread_Pool);

		for (int i = 0; i < _nThread_Pool; i++)
		{
			_io_service.push_back(boost::make_shared<io_service>());
			_work.emplace_back(boost::make_shared<io_service::work>(*_io_service[i]));
			_thread_Pool.push_back(boost::thread(boost::bind(&boost::asio::io_service::run, _io_service[i])));
			_thread_Pool.at(i).detach();
		}

		while (true)
		{
			socket_ptr sock(new ip::tcp::socket(_service));
			_acceptor->accept(*sock);

			balancer(sock);
		}
	}
private:
	void balancer(socket_ptr sock)
	{
		static int	nTask = 0,
			i = 0;

		if (nTask < 2)
		{
			_io_service[i]->post(boost::bind(&Server_Http::handler_request, this, sock));
			nTask++;
		}
		else
		{
			i++;
			nTask = 0;
			if (i == _nThread_Pool) i = 0;
		}
	}

	void handler_request(socket_ptr sock)
	{
		char data[1024];
		boost::system::error_code ec;

		size_t length = sock->read_some(buffer(data), ec); // читаем запрос от клиента
		if (!ec)
		{
			//string s = "TXT_" + boost::lexical_cast<std::string>(boost::this_thread::get_id()) + ".txt";
			//Logger* lg = new Logger(s);
			//
			//string st = "";
			//st += "id_   - "; 
			//st += boost::lexical_cast<std::string>(boost::this_thread::get_id());
			//
			//lg->log(st, stoi(boost::lexical_cast<std::string>(boost::this_thread::get_id())));
			//st.clear();
			// 

			auto idThread = std::stol(to_string(boost::this_thread::get_id()),
				nullptr,
				16);

			string message(data, length);
			string response;
			if (message.find("GET /home/") != string::npos) {
				response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
				response += "<html><body><h1 style=\"text-align:center;\">Home</h1></body></html>\n";
			}
			else if (message.find("GET /admin/") != string::npos) {
				response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
				response += "<html><body><h1 style=\"text-align:center;\">Admin</h1></body></html>\n";
			}
			else if (message.find("GET /all/") != string::npos) {
				response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
				response += "<html><body><h1 style=\"text-align:center;\">All</h1></body></html>\n";
			}
			else {
				response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n";
				response += "<html><body><h1 style=\"text-align:center;\">404 Not Found - </h1></body></html>\n";
			}


			write(*sock, buffer(response), ec); // отправляем ответ клиенту
			sock->shutdown(ip::tcp::socket::shutdown_both, ec); // закрываем соединение
		}
	}
};

int main()
{
//	io_service service;
	Server_Http serv(3000);

	serv.start();
	cout << "1\n";
	boost::this_thread::sleep(boost::posix_time::seconds(10));
	cout << "2\n";
	serv.stop();
	return 0;
}