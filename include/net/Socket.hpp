#pragma once

#include <span>
#include <optional>

#include "net/Native.hpp"
#include "net/IpAddress.hpp"
#include "net/Endpoint.hpp"

namespace net
{
	enum class AddressFamily
	{
		IPv4 = AF_INET,
		IPv6 = AF_INET6
	};

	enum class SocketType
	{
		Stream = SOCK_STREAM,
		Dgram = SOCK_DGRAM
	};

	enum class Protocol
	{
        Ip = IPPROTO_IP,
		Tcp = IPPROTO_TCP,
		Udp = IPPROTO_UDP
	};

	enum class OptionLevel
	{
		IP = SOL_IP,
		IPv6 = SOL_IPV6,
		Socket = SOL_SOCKET,
	};

	enum class OptionName
	{
		// Socket Level
		Linger = SO_LINGER,
		ReuseAddress = SO_REUSEADDR,
		SendBuffer = SO_SNDBUF,
		RecvBuffer = SO_RCVBUF,
		Broadcast = SO_BROADCAST,

#ifdef _WIN32
        UpdateAcceptContext = SO_UPDATE_ACCEPT_CONTEXT,
        UpdateConnectContext = SO_UPDATE_CONNECT_CONTEXT,
#endif
        // IP Level
		TTL = 4,

		// Tcp Level
		NoDelay = TCP_NODELAY
    };

	struct Linger
	{
		bool enabled;
		int time;
	};

    class Context;

	class Socket
    {
	public:
		Socket();
		Socket(Protocol pt);
		Socket(const Socket& sock);
		Socket(Socket&& sock) noexcept;
		~Socket();
	public:
		void close();
        void create(Protocol pt = Protocol::Ip);

		void setHandle(SOCKET s);

		bool bind(Endpoint ep);
		bool listen(int backlog = SOMAXCONN) const;
	public:
		[[nodiscard]] SOCKET getHandle() const;
        [[nodiscard]] std::optional<Endpoint> getRemoteEndpoint() const;
        [[nodiscard]] std::optional<Endpoint> getLocalEndpoint() const;
	public:
		void disconnect();
		[[nodiscard]] Socket accept() const;
		bool connect(Endpoint ep);

		bool send(std::span<char> s) const;
		bool send(std::span<char> s, Endpoint target) const;

		int receive(std::span<char> s) const;
		int receive(std::span<char> s, Endpoint target) const;
    public:
        bool disconnect(Context* context) const;
        bool accept(Context *context) const;
        bool connect(Context* context);
        bool send(Context* context) const;
        bool receive(Context* context) const;
    public:
        template<class T>
        bool setOption(OptionLevel level, OptionName name, T value) const
        {
            if (_sock == INVALID_SOCKET)
                return false;
            return SOCKET_ERROR != setsockopt(_sock,
                                              static_cast<int>(level),
                                              static_cast<int>(name),
                                              reinterpret_cast<const char*>(&value),
                                              sizeof(T)
            );
        }
        template<class T>
        bool getOption(OptionLevel level, OptionName name, T& value) const
        {
            if (_sock == INVALID_SOCKET)
                return false;
            SOCKLEN optLen = sizeof(T);
            return SOCKET_ERROR != getsockopt(_sock, level, name, &value, &optLen);
        }

        bool setBlocking(bool isBlocking) const;
        bool setLinger(Linger linger) const;
        bool setBroadcast(bool isBroadcast) const;
        bool setReuseAddress(bool isReuseAddr) const;
        bool setNoDelay(bool isNoDelay) const;
        bool setTTL(int ttl) const;
        bool setSendBuffer(int size) const;
        bool setReceiveBuffer(int size) const;

		[[nodiscard]] bool isOpen() const;

        void BindEndpoint() const;
	public:
        bool operator==(const Socket& sock) const;
        bool operator==(Socket&& sock) const;

		Socket& operator=(const Socket& sock);
		Socket& operator=(Socket&& sock) noexcept;
    private:
		std::optional<Endpoint> _remoteEndpoint;
        std::optional<Endpoint> _localEndpoint;
		SOCKET _sock;
	};
}