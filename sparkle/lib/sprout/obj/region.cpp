#include <sparkle/sprout/obj/region.hpp>

namespace sprk
{
	void SproutRegion::add_node(const NodeRef node)
	{
		nodes.push_back(node);
	}

	void SproutRegion::add_child(const std::shared_ptr<SproutRegion>& child)
	{
		children.push_back(child);
		child->parent = std::weak_ptr(shared_from_this());
		child->region_depth = region_depth + 1;
	}

	void SproutRegion::remove_child(const std::shared_ptr<SproutRegion> &child)
	{
		auto it = std::find(children.begin(), children.end(), child);
		if (it != children.end())
			children.erase(it);
	}

	void SproutRegion::replace_nodes(std::vector<NodeRef> new_nodes)
	{
		nodes = std::move(new_nodes);
	}

	void SproutRegion::set_ctrl_deps(const NodeRef control)
	{
		ctrl_dep = control;
	}

	void SproutRegion::set_imm_dominator(const std::shared_ptr<SproutRegion>& dom)
	{
		imm_dom = dom;
		if (dom)
			dom->dom_regions.push_back(shared_from_this());
	}

	void SproutRegion::set_type(const RegionType t, const uint32_t id)
	{
		type = t;
		type_id = id;
	}

	bool SproutRegion::dominates(const std::shared_ptr<const SproutRegion> &other) const
	{
		if (!other)
			return false;

		if (other.get() == this)
			return true; /* shouldn't be cyclic reference */

		std::shared_ptr<SproutRegion> curr = other->imm_dom.lock();
		while (curr)
		{
			if (curr.get() == this)
				return true;
			curr = curr->imm_dom.lock();
		}
		return false;
	}

	std::shared_ptr<const SproutRegion> SproutRegion::find_common_dominator(const std::shared_ptr<SproutRegion> &other) const
	{
		if (!other)
			return shared_from_this();

		if (dominates(other))
			return shared_from_this();

		if (other->dominates(shared_from_this()))
			return other;

		std::shared_ptr<SproutRegion> curr = other->imm_dom.lock();
		while (curr)
		{
			if (curr->dominates(other))
				return curr;
			curr = curr->imm_dom.lock();
		}

		/* root region should dominate everything */
		return nullptr;
	}
}
