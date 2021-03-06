#pragma once

#include "ComponentMatrix.hpp"
#include "ComponentRange.hpp"
#include "ComponentScheme.hpp"
#include "EntitySchemeSet.hpp"

#include <functional>
#include <span>


namespace inl::game {


class Entity;


//------------------------------------------------------------------------------
// General system
//------------------------------------------------------------------------------
class SystemBase {
public:
	struct UpdateMarks {
		std::vector<size_t> sweep;
		std::vector<size_t> modify;
	};
	enum class eUpdateFlag {
		NONE,
		SWEEP,
		MODIFY,
	};

public:
	virtual ~SystemBase() = default;

	virtual const ComponentScheme& Scheme() const = 0;

	virtual void Run(float elapsed, Scene& scene) = 0;
	virtual void Run(float elapsed, EntitySchemeSet& entitySet, Scene& scene) = 0;
};


//------------------------------------------------------------------------------
// Specific system taking a set of components
//------------------------------------------------------------------------------
template <class DerivedSystem, class... ComponentTypes>
class System : public SystemBase {
public:
	const ComponentScheme& Scheme() const override final;

	using SystemBase::Run;

	void Run(float elapsed, Scene& scene) override final;
	void Run(float elapsed, EntitySchemeSet& entitySet, Scene& scene) override final;

	virtual UpdateMarks Update(float elapsed, ComponentRange<ComponentTypes...>& range);
	virtual void Modify(std::span<Entity* const> entities) {}
	virtual void Spawn(std::function<Entity&()> spawn, std::span<const Entity* const> entities) {}

private:
	template <size_t... Indices>
	UpdateMarks UpdateHelper(std::index_sequence<Indices...> indices, float elapsed, ComponentRange<ComponentTypes...>& range);
};


//------------------------------------------------------------------------------
// Specific System taking zero components
//------------------------------------------------------------------------------
template <class DerivedSystem>
class System<DerivedSystem> : public SystemBase {
public:
	const ComponentScheme& Scheme() const override final;

	using SystemBase::Run;

	void Run(float elapsed, Scene& scene) override final;
	void Run(float elapsed, EntitySchemeSet& entitySet, Scene& scene) override final;

	virtual void Update(float elapsed) = 0;
	virtual void Modify(Scene& scene) {}
};


//------------------------------------------------------------------------------
// Implementation -- System taking no components
//------------------------------------------------------------------------------

template <class DerivedSystem>
const ComponentScheme& System<DerivedSystem>::Scheme() const {
	static const ComponentScheme scheme;
	return scheme;
}


template <class DerivedSystem>
void System<DerivedSystem>::Run(float elapsed, Scene& scene) {
	Update(elapsed);
	Modify(scene);
}


template <class DerivedSystem>
void System<DerivedSystem>::Run(float elapsed, EntitySchemeSet& entitySet, Scene& scene) {
	throw InvalidCallException("Please call the other function with no entity set.");
}


//------------------------------------------------------------------------------
// Implementation -- System taking multiple components
//------------------------------------------------------------------------------
template <class DerivedSystem, class... ComponentTypes>
const ComponentScheme& System<DerivedSystem, ComponentTypes...>::Scheme() const {
	static const ComponentScheme scheme{ std::type_index(typeid(ComponentTypes))... };
	return scheme;
}


template <class DerivedSystem, class... ComponentTypes>
void System<DerivedSystem, ComponentTypes...>::Run(float elapsed, Scene& scene) {
	throw InvalidCallException("Please call the other function with entity set.");
}


template <class DerivedSystem, class... ComponentTypes>
void System<DerivedSystem, ComponentTypes...>::Run(float elapsed, EntitySchemeSet& entitySet, Scene& scene) {
	// Update
	ComponentRange<ComponentTypes...> range(entitySet.GetMatrix());
	UpdateMarks marks = Update(elapsed, range);

	// Modify
	std::vector<Entity*> modifySet;
	modifySet.reserve(marks.modify.size());
	for (auto index : marks.modify) {
		modifySet.push_back(&entitySet[index]);
	}
	Modify(modifySet);

	// Create
	const Entity* const* first = modifySet.data();
	Spawn([&scene]() -> Entity& { return scene.CreateEntity(); }, modifySet);

	// Sweep
	for (auto index : marks.sweep) {
		Entity& entity = entitySet[index];
		scene.DeleteEntity(entity);
	}
}


template <class DerivedSystem, class... ComponentTypes>
auto System<DerivedSystem, ComponentTypes...>::Update(float elapsed, ComponentRange<ComponentTypes...>& range) -> UpdateMarks {
	return UpdateHelper(std::make_index_sequence<sizeof...(ComponentTypes)>(), elapsed, range);
}


template <class DerivedSystem, class... ComponentTypes>
template <size_t... Indices>
auto System<DerivedSystem, ComponentTypes...>::UpdateHelper(std::index_sequence<Indices...> indices, float elapsed, ComponentRange<ComponentTypes...>& range) -> UpdateMarks {
	auto&& self = static_cast<DerivedSystem&>(*this);
	using SingleUpdateReturnT = decltype(self.UpdateEntity(elapsed, std::get<Indices>(*range.begin())...));
	static_assert(std::is_void_v<SingleUpdateReturnT> || std::is_same_v<SingleUpdateReturnT, eUpdateFlag>, "Single entity update function UpdateEntity returns either void or eUpdateFlag");

	UpdateMarks marks;
	size_t index = 0;
	for (auto refTuple : range) {
		if constexpr (std::is_same_v<SingleUpdateReturnT, void>) {
			self.UpdateEntity(elapsed, std::get<Indices>(refTuple)...);
		}
		else {
			eUpdateFlag flag = self.UpdateEntity(elapsed, std::get<Indices>(refTuple)...);
			switch (flag) {
				case eUpdateFlag::SWEEP: marks.sweep.push_back(index); break;
				case eUpdateFlag::MODIFY: marks.modify.push_back(index); break;
			}
			++index;
		}
	}
	return marks;
}



} // namespace inl::game