// #include "Managers/KafkaProducer.hpp"
// #include "Models/RequestMessage.hpp"
// #include <userver/kafka/producer.hpp>
// #include <userver/kafka/producer_component.hpp>
// #include <userver/components/component_base.hpp>
// #include <userver/components/component.hpp>


// namespace auth_service::managers
// {

//     using namespace auth_service::models;

//     Producer::Producer(const userver::components::ComponentConfig& config,
//          const userver::components::ComponentContext& context) : ComponentBase(config, context),
//          kafkaProducer_(context.FindComponent<userver::kafka::ProducerComponent>().GetProducer()) {}

//     auth_service::models::SendStatus Producer::Produce(const auth_service::models::RequestMessage& message)
//     {
//         try
//         {
//             kafkaProducer_.Send(message.topic, message.key, message.payload);
//             return SendStatus::kSuccess;
//         }
//         catch (const userver::kafka::SendException& e)
//         {
//             return e.IsRetryable() ? SendStatus::kErrorRetryable : SendStatus::kErrorNonRetryable;
//         }
//     }
// }