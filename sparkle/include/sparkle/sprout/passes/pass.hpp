#pragma once

#include <memory>
#include <sparkle/sprout/node.hpp>
#include <sparkle/sprout/region.hpp>
#include <sparkle/sprout/passes/pass.hpp>

namespace sprk
{
	class SproutPass
	{
	public:
		virtual ~SproutPass() = default;
		virtual void run(const std::shared_ptr<SproutRegion>& root,
			 std::vector<std::unique_ptr<SproutNode<>>>& nodes) = 0;
	};
}
