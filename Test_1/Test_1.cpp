//
//#include <iostream>
//#include <boost/bind.hpp>
//#include <boost/smart_ptr.hpp>
//#include <boost/asio.hpp>
//#include <boost/thread/thread.hpp>
//
//using boost::asio::ip::tcp;
//using namespace boost::asio;
//using namespace std;
//
//
//typedef boost::shared_ptr<tcp::socket> socket_ptr;
//const int max_length = 1024;
//
//void session(socket_ptr sock)
//{
//    try
//    {
//        for (;;)
//        {
//            char data[max_length];
//
//            boost::system::error_code error;
//            size_t length = sock->read_some(boost::asio::buffer(data), error);
//            if (error == boost::asio::error::eof)
//                break; // Connection closed cleanly by peer.
//            else if (error)
//                throw boost::system::system_error(error); // Some other error.
//            cout << data << endl;
//        //    boost::asio::write(*sock, boost::asio::buffer(data, length));
//        }
//    }
//    catch (std::exception& e)
//    {
//        std::cerr << "Exception in thread: " << e.what() << "\n";
//    }
//}
//
//void server(boost::asio::io_service& io_service, unsigned short port)
//{
//	tcp::acceptor a(io_service, tcp::endpoint(tcp::v4(), port));
//	for (;;)
//	{
//		socket_ptr sock(new tcp::socket(io_service));
//		a.accept(*sock);
//		boost::thread t(boost::bind(session, sock));
//	}
//
//}
//
//class Server
//{
//    Server() {};
//};
//
//int main()
//{
//	std::cout << "Start\n";
//
//	io_service service;
//
//	server(service, 3000);
//
//}

//#include <boost/bind.hpp>
//#include <boost/asio.hpp>
//#include <iostream>
//
//using boost::asio::ip::tcp;
//using boost::bind;
//using namespace boost;
//
//
//class connection : public std::enable_shared_from_this<connection>
//{
//	tcp::socket socket_;
//	boost::asio::streambuf request_buffer_;
//
//public:
//	typedef std::shared_ptr<connection> pointer;
//
//	static pointer create(boost::asio::io_service& io_service)
//	{
//		return pointer(new connection(io_service));
//		std::cout << "3 - " << &io_service << std::endl;
//	}
//
//	tcp::socket& socket()
//	{
//		return socket_;
//	}
//
//	void start()
//	{
//		boost::asio::async_read_until(
//			socket_,
//			request_buffer_,
//			"\r\n\r\n",
//			//"<!DOCTYPE html> \
//			//< html > \
//			//<head> \
//			//<title>Заголовок< / title> \
//			//< / head> \
//			//< body> \
//			//\"Hello World!\" \
//			//< / body > \
//			//< / html>",
//			boost::bind(
//				&connection::handle_read_headers,
//				shared_from_this(),
//				boost::asio::placeholders::error,
//				boost::asio::placeholders::bytes_transferred
//			)
//		);
//	}
//
//private:
//	connection(boost::asio::io_service& io_service)
//		: socket_(io_service),
//		request_buffer_() {}
//
//	void handle_read_headers(const boost::system::error_code& error, size_t bytes_transferred)
//	{
//		if (!error)
//		{
//			std::istream request_stream(&request_buffer_);
//			std::string request;
//			char data[] = { "HTTP/1.1 200 OK\r\n\
//							Version: HTTP/1.1\r\n\
//							Content-Type: text/html; charset=utf-8\r\n\
//							Content - Length: 140 \
//							\r\n\r\n\
//							<html> \
//							<head> \
//							<title>Заголовок</title> \
//							</head> \
//							<body> \
//							\"Hello World!\" \
//							</body > \
//							</html>" };
//
//			std::getline(request_stream, request);
//			std::cout << "new request: " << request << std::endl << &request_buffer_ << std::endl;
//			socket_.write_some(boost::asio::buffer(data, strlen(data)));
//			// TODO: handle the request
//
//			socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
//		}
//	}
//};
//
//
//class http_server
//{
//	boost::asio::io_service& io_service_;
//	tcp::acceptor acceptor_;
//
//public:
//	http_server(boost::asio::io_service& io_service, short port)
//		: io_service_(io_service),
//		acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
//	{
//		std::cout << "1 - " << &io_service << "	2 - " << &io_service_ << std::endl;
//		start_accept();
//	}
//
//private:
//	void start_accept()
//	{
//		auto new_connection = connection::create(io_service_);
//
//		acceptor_.async_accept(
//			new_connection->socket(),
//			boost::bind(
//				&http_server::handle_accept,
//				this,
//				new_connection,
//				boost::asio::placeholders::error
//			)
//		);
//	}
//
//	void handle_accept(std::shared_ptr<connection> new_connection, const boost::system::error_code& error)
//	{
//		if (error)
//		{
//			std::cout << "error: " << error << std::endl;
//			return;
//		}
//
//		new_connection->start();
//		start_accept();
//	}
//};
//
//int main(int argc, char* argv[])
//{
//	try
//	{
//		/*     if (argc != 2) {
//				 std::cerr << "Usage: http_server <port>\n";
//				 return 1;
//			 }*/
//
//		boost::asio::io_service io_service;
//
//		http_server server(io_service, 3000);
//		std::cout << "listening on port " << 3000 << std::endl;
//
//		io_service.run();
//	}
//	catch (std::exception& e) {
//		std::cerr << "Exception: " << e.what() << "\n";
//	}
//
//	return 0;
//}

////////////////////////////

//#include <queue>
//
//class ThreadPool 
//{
//
//public:
//	ThreadPool(size_t numThreads) 
//	{
//		for (size_t i = 0; i < numThreads; i++)
//			workers.push_back(boost::thread(boost::bind(&ThreadPool::workerThread, this)));
//	}
//
//	~ThreadPool() 
//	{
//		workQueue.push();
//		for (size_t i = 0; i < workers.size(); i++)
//			workers[i].join();
//	}
//
//	void addTask(boost::function<void()>& task) 
//	{
//		workQueue.push(task);
//	}
//
//private:
//	std::queue<boost::function<void()>> workQueue;
//	std::vector<boost::thread> workers;
//	boost::mutex queueMutex;
//	boost::condition_variable queueCondition;
//
//	void workerThread() {
//		while (true) {
//			// Wait for work to do
//			boost::unique_lock<boost::mutex> lock(queueMutex);
//			while (workQueue.empty()) {
//				queueCondition.wait(lock);
//			}
//
//			// Get the next task from the queue
//		//	boost::function<void()>& task = workQueue.front();
//			
//			auto& task = workQueue.front();
//			workQueue.pop();
//
//			// Exit if the task is NULL
//			if (!task) {
//				break;
//			}
//
//			// Run the task
//			task();
//		}
//	}
//};

//
//typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;
//io_service service;
//
//void handle_request(socket_ptr socket)
//{
//
//	static int i = 0;
//	cout << "[" << i << "]\n";
//	i++;
//
//	boost::system::error_code ec;
//	char data[1024];
//	size_t length = socket->read_some(buffer(data), ec); // читаем запрос от клиента
//
//
//	if (!ec)
//	{
//		string message(data, length);
//		cout << message << endl;
//		string response;
//		if (message.find("GET /home/") != string::npos) {
//			response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
//			response += "<html><body><h1 style=\"text-align:center;\">Home</h1></body></html>\n";
//		}
//		else if (message.find("GET /admin/") != string::npos) {
//			response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
//			response += "<html><body><h1 style=\"text-align:center;\">Admin</h1></body></html>\n";
//		}
//		else if (message.find("GET /all/") != string::npos) {
//			response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
//			response += "<html><body><h1 style=\"text-align:center;\">All</h1></body></html>\n";
//		}
//		else {
//			response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n";
//			response += "<html><body><h1 style=\"text-align:center;\">404 Not Found</h1></body></html>\n";
//		}
//		write(*socket, buffer(response), ec); // отправляем ответ клиенту
//		socket->shutdown(ip::tcp::socket::shutdown_both, ec); // закрываем соединение
//	}
//}
//
//
//void handle_connections()
//{
//	ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 3000));
//
//	while (true)
//	{
//		socket_ptr sock(new ip::tcp::socket(service));
//		acceptor.accept(*sock);
//		boost::thread t(boost::bind(handle_request, sock));
//		t.detach();
//	}
//}

#include <queue>
#include <iostream>
#include <fstream> 
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/function.hpp>

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

//typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;

using socket_ptr = boost::shared_ptr<ip::tcp::socket>;

struct Thread_Conf
{
	int id_ = 0;
	boost::thread thread_;
	std::queue<socket_ptr> task_;
};


class Server_Http
{

	boost::thread_group			_threadpool;
	vector<io_service>			_io_service;
	vector<io_service::work>	_work;

public:

	Server_Http(io_service& service, int port = 3000, int tp = boost::thread::hardware_concurrency() - 6)
	{
		ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), port));

		Listener(service, acceptor, tp);
	};

private:

	int Listener(io_service& service, ip::tcp::acceptor& acceptor, int tp)
	{
		_io_service.reserve(tp);
		_work.reserve(tp);

		for (int i = 0; i < tp; i++)
		{
			_work.push_back(io_service::work(_io_service[i]));
			_threadpool.create_thread(boost::bind(&boost::asio::io_service::run, &_io_service[i]));
		}

		while (true)
		{
			socket_ptr sock(new ip::tcp::socket(service));
			acceptor.accept(*sock);

			int id = 0;
			i = std::numeric_limits<int>::max();

			for (int j = 0; j < tp; j++)
				if (Thread_Pool.at(j).task_.size() < i)
				{
					i = Thread_Pool.at(j).task_.size();
					id = Thread_Pool.at(j).id_;
				}

			Thread_Pool.at(id).task_.push(sock);
		}
	}

	void Load_Balancer()
	{

	}


	void Server_Process(int id)
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
};

int main()
{
	//	handle_connections();
	io_context f;
	io_service service;
	Server_Http serv(service, 3000);

	return 0;
}