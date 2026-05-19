#include <CQLDriver/Common/CommonDefinitions.hpp>
#include "./SSLConnector.hpp"

namespace cql {
	/* Setup the connection */
	seastar::future<seastar::connected_socket> SSLConnector::connect(
		const NodeConfiguration&,
		const seastar::socket_address& address) const {
		seastar::tls::tls_options opts;
        if (initialized_.available() && !initialized_.failed()) [[likely]] {
            // fast path
            return seastar::tls::connect(certificates_, address, std::move(opts));
        } else {
            // slow path
            // Захватываем opts по значению, так как лямбда выполнится позже
            return initialized_.get_future().then([c = certificates_, a = address, o = std::move(opts)]() mutable {
                return seastar::tls::connect(c, a, std::move(o));
            });
        }
	}

	/** Constructor */
	SSLConnector::SSLConnector() :
		certificates_(seastar::make_shared<seastar::tls::certificate_credentials>()),
		initialized_(certificates_->set_system_trust()) { }
}
