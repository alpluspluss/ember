#pragma once

#include <memory>
#include <sparkle/sprout/obj/node.hpp>
#include <sparkle/sprout/obj/region.hpp>
#include <utility>

namespace sprk
{
    template<NodeType Type>
    class SproutBuilder
    {
    public:
        SproutBuilder(NodeRef id)
        {
            node = {};
            node.id = id;
            node.type = Type;
        }

        template<NodeProps Prop>
        SproutBuilder& tag()
        {
            node.props |= Prop;
            return *this;
        }

        template<typename T>
        SproutBuilder& value(T&& v)
        {
            node.value = std::forward<T>(v);
            return *this;
        }

        SproutNode<> build()
        {
            return std::move(node);
        }

    private:
        SproutNode<> node;
    };

    /* equivalent to IR module*/
    class SproutRegionBuilder
    {
    public:
        SproutRegionBuilder(std::string name) : region(std::make_shared<SproutRegion>(std::move(name))) {}

        SproutRegionBuilder& add(NodeRef node)
        {
            return *this;
        }

        SproutRegionBuilder& add(const std::vector<NodeRef>& nodes)
        {
            return *this;
        }

        SproutRegionBuilder& add(const std::shared_ptr<SproutRegion>& child)
        {
            return *this;
        }

        SproutRegionBuilder& remove(const std::shared_ptr<SproutRegion>& child)
        {
            return *this;
        }

        SproutRegionBuilder& replace(const std::vector<NodeRef>& new_nodes)
        {
            return *this;
        }

        SproutRegionBuilder& ctrl_dep(NodeRef control) 
        {
            region->set_ctrl_deps(control);
            return *this;
        }
    
        SproutRegionBuilder& imm_dominator(const std::shared_ptr<SproutRegion>& dom) 
        {
            region->set_imm_dominator(dom);
            return *this;
        }
    
        SproutRegionBuilder& type(RegionType type, uint32_t id = 0) 
        {
            region->set_type(type, id);
            return *this;
        }

        std::shared_ptr<SproutRegion> build() 
        {
            return region;
        }

    private:
        std::shared_ptr<SproutRegion> region;
    };
}
