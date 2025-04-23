#include "Example.hpp"

#include <flow/core/NodeFactory.hpp>

#ifdef FLOW_WINDOWS
#ifdef FLOW_SIMPLE_EXPORT
#define FLOW_SIMPLE_API __declspec(dllexport) FLOW_CORE_CALL
#else
#define FLOW_SIMPLE_API __declspec(dllimport) FLOW_CORE_CALL
#endif
#else
#define FLOW_SIMPLE_API
#endif

extern "C"
{
    namespace flow::simple
    {
    void FLOW_SIMPLE_API RegisterModule(std::shared_ptr<flow::NodeFactory> factory)
    {
        factory->RegisterNodeClass<ExampleNode>("example", "Example");
    }

    void FLOW_SIMPLE_API UnregisterModule(std::shared_ptr<flow::NodeFactory> factory)
    {
        factory->UnregisterNodeClass<ExampleNode>("example");
    }
    } // namespace flow::simple
}
