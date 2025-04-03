#pragma once

#include <set>
#include <sparkle/sprout/passes/pass.hpp>

namespace sprk
{
	class DCEPass final : public SproutPass
	{
	public:
		DCEPass() = default;

		void run(const std::shared_ptr<SproutRegion> &root,
		         std::vector<std::unique_ptr<SproutNode<> > > &nodes) override;

		[[nodiscard]] std::set<NodeRef> get_dead_nodes() const
		{
			return dead_nodes;
		}

		[[nodiscard]] std::set<NodeRef> get_alive_nodes() const
		{
			return alive_nodes;
		}

		void dump_results(const std::set<NodeRef> &dead_nodes,
		                  const std::vector<std::unique_ptr<SproutNode<> > > &nodes,
		                  const std::shared_ptr<SproutRegion> &root,
		                  bool colorize = true);

	private:
		std::set<NodeRef> dead_nodes;
		std::set<NodeRef> alive_nodes;
	};
}
