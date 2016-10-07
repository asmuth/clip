/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#ifndef _STX_NET_TCPCONNECTION_H
#define _STX_NET_TCPCONNECTION_H
#include <stdlib.h>
#include "metricd/transport/http/inetaddr.h"
#include "metricd/transport/http/taskscheduler.h"

namespace fnordmetric {
namespace http {

class TCPConnection {
public:

  /**
   * Open a new tcp (client) connection and block until the connection is
   * established or raise an exception if the connction fails
   */
  static std::unique_ptr<TCPConnection> connect(const InetAddr& addr);

  /**
   * Open a new tcp (client) connection and return immediately. IMPORTANT
   * the caller must call connection->checkErrors(); after the onReady callback
   * fires!
   */
  static void connectAsync(
      const InetAddr& addr,
      TaskScheduler* scheduler,
      std::function<void(std::unique_ptr<TCPConnection> conn)> on_ready);

  TCPConnection(int fd);
  ~TCPConnection();
  int fd() const;

  size_t read(void* dst, size_t size);
  size_t write(const void* data, size_t size);
  void close();
  void setNonblocking(bool nonblocking = true);

  /**
   * This will raise an exception if there are any pending errors on the
   * connection
   */
  void checkErrors() const;

protected:
  void connectImpl(const InetAddr& addr);

  bool closed_;
  int fd_;
};

}
}
#endif
