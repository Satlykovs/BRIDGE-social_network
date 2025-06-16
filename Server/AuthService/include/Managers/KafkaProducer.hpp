// #ifndef PRODUCER_HPP
// #define PRODUCER_HPP

// #include <userver/components/component_fwd.hpp>
// #include <userver/components/component_base.hpp>
// #include  <userver/kafka/producer.hpp>
// #include "Models/RequestMessage.hpp"
// namespace auth_service::managers
// {

//     class Producer final  : public userver::components::ComponentBase
//     {
//         public:
//             static constexpr std::string_view kName = "producer";

//             Producer(const userver::components::ComponentConfig& config,
//             const userver::components::ComponentContext& context);

//             auth_service::models::SendStatus Produce(const
//             auth_service::models::RequestMessage& message);
//         private:
//             const userver::kafka::Producer& kafkaProducer_;

//     };
// }
// #endif