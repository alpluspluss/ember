#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <sparkle/ir/obj/node.hpp>

namespace sprk
{
    enum class RegionType : uint8_t
    {
        ROOT,
        FUNCTION,
        BASIC_BLOCK,
        LOOP_BODY,
        BR_THEN,
        BR_ELSE
    };

    class SproutRegion : public std::enable_shared_from_this<SproutRegion>
    {
    public:
        explicit SproutRegion(std::string region_name) : name(std::move(region_name)) {}

        /* analysis helper */
        bool dominates(const std::shared_ptr<const SproutRegion>& other) const;

        std::shared_ptr<const SproutRegion> find_common_dominator(const std::shared_ptr<SproutRegion>& other) const;

        /* setter */
        void add_node(NodeRef node);

        void add_child(const std::shared_ptr<SproutRegion>& child);

        void remove_child(const std::shared_ptr<SproutRegion>& child);

        void replace_nodes(std::vector<NodeRef> new_nodes);

        void set_ctrl_deps(NodeRef control);

        void set_imm_dominator(const std::shared_ptr<SproutRegion> &dom);

        void set_type(RegionType t, uint32_t id = 0);

        const std::string& get_name() const
        {
            return name;
        }

        const std::vector<NodeRef>& get_nodes() const 
        { 
            return nodes; 
        }

        const std::vector<std::shared_ptr<SproutRegion>>& get_children() const 
        { 
            return children; 
        }

        std::shared_ptr<SproutRegion> get_parent() const
        { 
            return parent.lock(); 
        }

        NodeRef get_ctrl_dep() const
        {
            return ctrl_dep; 
        }

        std::shared_ptr<SproutRegion> get_imm_dom() const 
        { 
            return imm_dom.lock(); 
        }

        const std::vector<std::shared_ptr<SproutRegion>>& get_dom_regions() const 
        { 
            return dom_regions; 
        }

        RegionType get_type() const
        {
            return type;
        }

        uint32_t get_type_id() const
        {
            return type_id;
        }

        uint32_t get_depth() const 
        { 
            return region_depth; 
        }
    private:
        std::vector<NodeRef> nodes;
        std::vector<std::shared_ptr<SproutRegion>> children;
        std::vector<std::shared_ptr<SproutRegion>> dom_regions; /* dominance info */
        std::string name;
        std::weak_ptr<SproutRegion> parent;
        std::weak_ptr<SproutRegion> imm_dom; /* dominance info */
        NodeRef ctrl_dep = NULL_REF;

        /* metadata */
        uint32_t type_id = 0;
        uint32_t region_depth = 0;
        RegionType type = RegionType::ROOT;
    };

    inline constexpr RegionType REGION_ROOT = RegionType::ROOT;
    inline constexpr RegionType REGION_FUCNTION = RegionType::FUNCTION;
    inline constexpr RegionType REGION_BB = RegionType::BASIC_BLOCK;
    inline constexpr RegionType REGION_LOOP = RegionType::LOOP_BODY;
    inline constexpr RegionType REGION_BRANCH_THEN = RegionType::BR_THEN;
}
