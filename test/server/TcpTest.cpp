#include "TcpTest.h"
#include "TcpServer.h"

TcpTest::TcpTest(TestLoop* loop, long conn_id)
: loop_(loop)
, tcp_(loop->getEventLoop())
, conn_id_(conn_id)
{
    
}

int TcpTest::attachFd(SOCKET_FD fd)
{
    tcp_.setReadCallback([this] (int err) { onReceive(err); });
    tcp_.setWriteCallback([this] (int err) { onSend(err); });
    tcp_.setErrorCallback([this] (int err) { onClose(err); });
    
    return tcp_.attachFd(fd);
}

int TcpTest::close()
{
    return tcp_.close();
}

void TcpTest::onSend(int err)
{
    
}

void TcpTest::onReceive(int err)
{
    char buf[4096] = {0};
    do
    {
        int bytes_read = tcp_.receive((uint8_t*)buf, sizeof(buf));
        if(bytes_read < 0) {
            tcp_.close();
            loop_->removeObject(conn_id_);
            return ;
        } else if (0 == bytes_read){
            break;
        }
        int ret = tcp_.send((uint8_t*)buf, bytes_read);
        if(ret < 0) {
            tcp_.close();
            loop_->removeObject(conn_id_);
        }
    } while(true);
}

void TcpTest::onClose(int err)
{
    printf("TcpTest::onClose, err=%d\n", err);
    loop_->removeObject(conn_id_);
}