#pragma once

#include <unordered_map>
#include <memory>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

//
// Overview: A class that is the main controller for a resource.
// Note: You can have many resource controllers, not just one.
//
// Usage:
//  CResourceController<sf::Texture> rc;
//
//  {
//    CResource<sf::Texture> res;
//    res.Load(rc, "PlayerTexture");
//    Draw(res.Get());
//  }
//
template <typename T>
class CResourceController
{
public:
	CResourceController();
	~CResourceController();

	//
	// This function frees any resources that have
	// no other instances linked to them.
	void FreeUnused();

	//
	// This function frees the resource if it has
	// no other instances linked to them.
	void FreeUnused(const std::string& path);

	template <typename>
	friend class CResource;
	template <typename>
	friend class CCleanResource;

private:
	std::shared_ptr<T> Load(const std::string& path);
	void Unload(const std::string& path);

private:
	std::unordered_map<std::string, std::shared_ptr<T>> m_resources;
};

//
// Overview: Holds a resource and is tied to a controller for garbage collecting,
// but does not clean it up it when it goes out of scope.
//
template <typename T>
class CResource
{
public:
	CResource();
	CResource(CResourceController<T>& controller,
			  const std::string& path);
	~CResource();

	void Load(CResourceController<T>& controller,
			  const std::string& path);
	T& Get();
	const T& Get() const;

private:
	std::shared_ptr<T> m_resource;

};

//
// Overview: Holds a resource and is tied to a controller for garbage collecting,
// but DOES clean it up it when it goes out of scope.
// Note: The controller used to load this must still be in its scope, otherwise
// the pointer in here will be pointing to bad memory.
//
template <typename T>
class CCleanResource
{
public:
	CCleanResource();
	CCleanResource(CResourceController<T>& controller,
				   const std::string& path);
	~CCleanResource();

	void Load(CResourceController<T>& controller,
			  const std::string& path);
	T& Get();
	const T& Get() const;

private:
	std::shared_ptr<T> m_resource;
	CResourceController<T>* m_controller;
	std::string m_path;

};

//
// CResourceController Class Functions
//
// Public Functions //
template <typename T>
inline CResourceController<T>::CResourceController()
{

}
template <typename T>
inline CResourceController<T>::~CResourceController()
{

}
template <typename T>
inline void CResourceController<T>::FreeUnused()
{
	for (auto it = m_resources.begin(); it != m_resources.end(); it++) {
		if (it.second.use_count() == 1) {
			it = m_resources.erase(it);
		}
	}
}
template <typename T>
inline void CResourceController<T>::FreeUnused(const std::string& path)
{
	//if (m_resources.find(path) != m_resources.end()) {
		if (m_resources.at(path).use_count() == 1) {
			m_resources.erase(path);
		}
	//}
}

// Private Functions //
template <typename T>
inline std::shared_ptr<T> CResourceController<T>::Load(const std::string& path)
{
	return m_resources[path];
}
template <>
inline std::shared_ptr<sf::Texture> CResourceController<sf::Texture>::Load(const std::string& path)
{
	if (m_resources.find(path) == m_resources.end()) {
		m_resources[path] = std::make_shared<sf::Texture>();
		m_resources[path]->loadFromFile(path);
	}

	return m_resources[path];
}

template <>
inline std::shared_ptr<sf::SoundBuffer> CResourceController<sf::SoundBuffer>::Load(const std::string& path)
{
	if (m_resources.find(path) == m_resources.end()) {
		m_resources[path] = std::make_shared<sf::SoundBuffer>();
		m_resources[path]->loadFromFile(path);
	}

	return m_resources[path];
}

template <>
inline std::shared_ptr<sf::Music> CResourceController<sf::Music>::Load(const std::string& path)
{
	if (m_resources.find(path) == m_resources.end()) {
		m_resources[path] = std::make_shared<sf::Music>();
		m_resources[path]->openFromFile(path);
	}

	return m_resources[path];
}
template <typename T>
inline void CResourceController<T>::Unload(const std::string& path)
{
	if (m_resources.find(path) != m_resources.end()) {
		m_resources.erase(path);
	}
}

//
// CCleanResource Class Functions
//
// Public Functions //
template<typename T>
inline CCleanResource<T>::CCleanResource()
	: m_controller(nullptr), m_path("")
{

}
template<typename T>
inline CCleanResource<T>::CCleanResource(CResourceController<T>& controller,
								  const std::string& path)
{
	Load(controller, path);
}
template<typename T>
inline CCleanResource<T>::~CCleanResource()
{
	if (m_controller != nullptr) {
		m_controller->FreeUnused(m_path);
	}
}
template<typename T>
inline void CCleanResource<T>::Load(CResourceController<T>& controller,
							 const std::string& path)
{
	m_path = path;
	m_resource = controller.Load(path);
	m_controller = &controller;
}
template<typename T>
inline T& CCleanResource<T>::Get()
{
	return *m_resource.get();
}
template<typename T>
inline const T& CCleanResource<T>::Get() const
{
	return *m_resource.get();
}

//
// CResource Class Functions
//
// Public Functions //
template<typename T>
inline CResource<T>::CResource()
{

}
template<typename T>
inline CResource<T>::CResource(CResourceController<T>& controller,
						const std::string& path)
{
	Load(controller, path);
}
template<typename T>
inline CResource<T>::~CResource()
{

}
template<typename T>
inline void CResource<T>::Load(CResourceController<T>& controller,
						const std::string& path)
{
	m_resource = controller.Load(path);
}
template<typename T>
inline T& CResource<T>::Get()
{
	return *m_resource.get();
}
template<typename T>
inline const T& CResource<T>::Get() const
{
	return *m_resource.get();
}
