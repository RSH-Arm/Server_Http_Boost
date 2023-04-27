
#define DEBUG false


#include <queue>
#include <iostream>
#include <fstream> 
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/function.hpp>

using namespace std;
using namespace boost::asio;


using socket_ptr		= boost::shared_ptr<ip::tcp::socket>;
using io_service_ptr	= boost::shared_ptr<boost::asio::io_service>;
using work_ptr			= boost::shared_ptr<boost::asio::io_service::work>;


class Server_Http
{
	int										_nThread_Pool;

	vector<work_ptr>						_work;
	vector<boost::thread>					_thread_Pool;
	vector<io_service_ptr>					_io_service;

public:

	Server_Http(io_service& service,
		int					port = 3000,
		int					tp = boost::thread::hardware_concurrency() - 6)
	{
		_nThread_Pool = tp;
		ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), port));
		listener(service, acceptor);
	};

private:

	void listener(io_service& service, ip::tcp::acceptor& acceptor)
	{
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
			socket_ptr sock(new ip::tcp::socket(service));
			acceptor.accept(*sock);

			balancer(sock);
		}
	}

	void balancer(socket_ptr sock)
	{
		static int	nTask = 0,
			i = 0;

		if (nTask < 100)
		{
			_io_service[i]->post(boost::bind(&Server_Http::handler_request, this, sock));
			nTask++;
		}
		else
		{
			nTask = 0;
			i++;
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
				response += "<html><body><h1 style=\"text-align:center;\">404 Not Found</h1></body></html>\n";
			}


			write(*sock, buffer(response), ec); // отправляем ответ клиенту
			sock->shutdown(ip::tcp::socket::shutdown_both, ec); // закрываем соединение
		}
	}


#if DEBUG

	void server_Process(int id)
	{
		string s = "TXT_" + to_string(id) + ".txt";
		Logger* lg = new Logger(s);
		int i = 0;
		while (true)
			if (this->Thread_Pool.at(id).task_.empty());
			else
			{
				socket_ptr socket = this->Thread_Pool.at(id).task_.front();

				boost::system::error_code ec;
				char data[1024];
				size_t length = socket->read_some(buffer(data), ec); // читаем запрос от клиента


				if (!ec)
				{
					string message(data, length);

					string st = "";

					//		lg->log(message);
					//		lg->log("THREAD :"); 

					st += "id_   - "; st += to_string(this->Thread_Pool.at(id).id_);
					lg->log(st, i);
					st.clear();

					st += "task_ - "; st += to_string(this->Thread_Pool.at(id).task_.size());
					lg->log(st, i);
					st.clear();


					st += "sock_ - "; st += to_string(socket->native_handle()); st += "\n";
					lg->log(st, i);
					st.clear();

					i++;

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
						response += "<html><body><h1 style=\"text-align:center;\">404 Not Found</h1></body></html>\n";
					}

					this->Thread_Pool.at(id).task_.pop();

					write(*socket, buffer(response), ec); // отправляем ответ клиенту
					socket->shutdown(ip::tcp::socket::shutdown_both, ec); // закрываем соединение
					//	boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
				}
			}
	}
#endif // DEBUG

};

int main()
{
	io_service service;
	Server_Http serv(service, 3000);

	return 0;
}