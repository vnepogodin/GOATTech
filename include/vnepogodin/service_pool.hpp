/*
 * Copyright (C) 2021 Vladislav Nepogodin
 *
 * This file is part of SportTech overlay project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef SERVICE_POOL_HPP_
#define SERVICE_POOL_HPP_

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <thread>
#include <vector>

namespace vnepogodin {
namespace http {

/// A pool of io_service objects.
class service_pool {
 public:
    // Constructor.
    explicit service_pool(std::size_t pool_size)
      : m_services(pool_size), m_work(pool_size) {
        if (pool_size == 0)
            throw std::runtime_error("service_pool size is 0");

        // Give all the io_services work to do so that their run() functions will not
        // exit until they are explicitly stopped.
        for (std::size_t i = 0; i < pool_size; ++i) {
            service_ptr io_service(new boost::asio::io_service);
            work_ptr work(new boost::asio::io_service::work(*io_service));

            m_services[i] = io_service;
            m_work[i] = work;
        }
    }

    virtual ~service_pool() = default;

    void run() {
        // Create a pool of threads to run in parallel.
        //
        std::vector<std::shared_ptr<std::thread>> threads(m_services.size());
        for (std::size_t i = 0; i < threads.size(); ++i) {
            std::shared_ptr<std::thread> thread(new std::thread(
                boost::bind(&boost::asio::io_service::run, m_services[i])));
            threads[i] = std::move(thread);
        }

        // Wait for threads.
        //
        for (auto& thread : threads) {
            thread->join();
        }
    }
    void stop() {
        // Explicitly stop all io_services.
        for (auto& service : m_services) {
            service->stop();
        }
    }
    auto get_service() noexcept -> boost::asio::io_service& {
        // Use a round-robin scheme to choose the next io_service to use.
        boost::asio::io_service& service = *m_services[m_next];
        ++m_next;
        if (m_next == m_services.size())
            m_next = 0;
        return service;
    }
    service_pool(const service_pool&) = delete;
    constexpr auto operator=(const service_pool&) -> service_pool& = delete;

 private:
    using service_ptr = std::shared_ptr<boost::asio::io_service>;
    using work_ptr = std::shared_ptr<boost::asio::io_service::work>;

    // The next service
    //
    std::size_t m_next{};

    std::vector<service_ptr> m_services;

    // Keeps running.
    //
    std::vector<work_ptr> m_work;
};

}  // namespace http
}  // namespace vnepogodin

#endif  // SERVICE_POOL_HPP_
