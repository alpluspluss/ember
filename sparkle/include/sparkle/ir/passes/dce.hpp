#pragma once

#include <set>
#include <sparkle/ir/passes/pass.hpp>

namespace sprk
{
    struct DCEStats
    {
        size_t total = 0;
        size_t dead = 0;
        size_t alive = 0;
        size_t removed = 0;
    };

    class DCEPass final : public SproutPass
    {
    public:
        DCEPass() = default;
        
        void run(const std::shared_ptr<SproutRegion>& root,
                std::vector<std::unique_ptr<SproutNode<>>>& nodes) override;
                
        [[nodiscard]] std::set<NodeRef> get_dead_nodes() const
        {
            return dead_nodes;
        }
        
        [[nodiscard]] std::set<NodeRef> get_alive_nodes() const
        {
            return alive_nodes;
        }
        
        [[nodiscard]] DCEStats get_stats() const
        {
            return stats;
        }

        static void dump_results(const std::set<NodeRef> &dead_nodes,
                               const std::vector<std::unique_ptr<SproutNode<> > > &nodes,
                               const std::shared_ptr<SproutRegion> &root,
                               bool colorize = true);
        
    private:
        std::set<NodeRef> dead_nodes;
        std::set<NodeRef> alive_nodes;
        DCEStats stats;
    };
}
