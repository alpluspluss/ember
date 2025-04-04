#pragma once

#include <sparkle/ir/obj/node.hpp>
#include <sparkle/ir/obj/region.hpp>

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
