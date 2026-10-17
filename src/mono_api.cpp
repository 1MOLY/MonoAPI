//
// Created by Lukas on 17 Okt. 2026.
//
#include "mono_api.hpp"
#include "_names.hpp"


MonoAPI::MonoAPI(const std::string &mono_dll_name)
{
	if (mono_dll_name.empty())
		if (!FindMonoDLLName())
			throw std::exception("[ MONO ] Failed to find mono dll name!");

	#define LOAD(name) \
		fn_##name = (_mono::fn_##name##_t)GetProcAddress(m_mono, #name); \
		if (!fn_##name) { \
			throw std::exception("[ MONO ] Failed to load mono api!"); \
		}

		LOAD(mono_get_root_domain)
		LOAD(mono_thread_attach)
		LOAD(mono_assembly_get_image)
		LOAD(mono_image_get_name)
		LOAD(mono_class_from_name)
		LOAD(mono_class_get_field_from_name)
		LOAD(mono_class_vtable)
		LOAD(mono_field_static_get_value)
		LOAD(mono_field_static_set_value)
		LOAD(mono_field_get_value)
		LOAD(mono_field_set_value)
		LOAD(mono_field_get_offset)
		LOAD(mono_assembly_foreach)

	#undef LOAD

	fn_mono_class_get_static_field_data =
		reinterpret_cast<_mono::fn_mono_class_get_static_field_data_t>(
			GetProcAddress(m_mono, "mono_class_get_static_field_data")
		);

	fn_mono_class_getStaticFieldAddress =
		reinterpret_cast<_mono::fn_mono_class_getStaticFieldAddress_t>(
			GetProcAddress(m_mono, "mono_class_getStaticFieldAddress")
		);

	m_domain = fn_mono_get_root_domain();
	if (!m_domain)
		throw std::exception("[ MONO ] Failed to parse mono root domain!");

	fn_mono_thread_attach(m_domain);
	m_initialized = TRUE;
}

BOOL MonoAPI::Start()
{
	if (!m_initialized) return FALSE;

	if (!FindMainImage())
		throw std::exception("[ MONO ] Failed to find mono main image!");

	return TRUE;
}

_mono::MonoClass *MonoAPI::FindClass(const std::string &_class, const std::string &_namespace)
{
	if (!m_initialized)
	{
		std::cerr << "[ MONO ] Failed to find class with mono not initialized!\n";
		return nullptr;
	}

	_mono::MonoImage* image = GetMainImage();
	if (!image)
	{
		std::cerr << "[ MONO ] Failed to find class as there is no valid main image!\n";
		return nullptr;
	}

	_mono::MonoClass* classOut = fn_mono_class_from_name(image, _namespace.c_str(), _class.c_str());
	if (!classOut)
	{
		std::cerr << "[ MONO ] Failed to find class with name: " << _namespace << '.' << _class << '\n';
		return nullptr;
	}

	return classOut;
}

_mono::MonoVTable *MonoAPI::GetVTable(_mono::MonoClass *_class)
{
	if (!m_initialized)
	{
		std::cerr << "[ MONO ] Failed to get VTable with mono not initialized!\n";
		return nullptr;
	}

	_mono::MonoVTable* table = fn_mono_class_vtable(m_domain, _class);
	if (!table)
	{
		std::cerr << "[ MONO ] Failed to get VTable";
		return nullptr;
	}
	return table;
}

uintptr_t MonoAPI::GetStaticPointer(_mono::MonoClass *_class, const std::string &fieldName)
{
	if (!m_initialized)
	{
		std::cerr << "[ MONO ] Failed to get static pointer with mono not initialized!\n";
		return 0;
	}

	uintptr_t ptr{ 0 };
	GetStaticField(_class, fieldName, ptr);
	return ptr;
}

uintptr_t MonoAPI::GetStaticBase(_mono::MonoClass *_class)
{
	if (!m_initialized)
	{
		std::cerr << "[ MONO ] Failed to get static base with mono not initialized!\n";
		return 0;
	}

	if (fn_mono_class_getStaticFieldAddress)
		return fn_mono_class_getStaticFieldAddress(m_domain, _class);

	if (!fn_mono_class_get_static_field_data)
		return 0;

	_mono::MonoVTable* vTable = fn_mono_class_vtable(m_domain, _class);
	if (!vTable) {
		std::cerr << "[ MONO ] Failed to get VTable in ( GetStaticBase )!\n";
		return 0;
	}
	return reinterpret_cast<uintptr_t>(fn_mono_class_get_static_field_data(vTable));
}

BOOL MonoAPI::FindMonoDLLName()
{
	for (auto& [_, name] : _mono::mono_names)
	{
		const HANDLE& mod = GetModuleHandleA(name.data());
		if (mod == INVALID_HANDLE_VALUE || mod == nullptr)
			continue;

		CloseHandle(mod);

		m_mono_dll_name = name.data();
		return TRUE;
	}

	return FALSE;
}

BOOL MonoAPI::FindMainImage()
{
	if (m_mainImage) return TRUE;

	struct FindData
	{
		_mono::MonoImage* image { nullptr };
		_mono::fn_mono_assembly_get_image_t fn_getImg { nullptr };
		_mono::fn_mono_image_get_name_t fn_getName { nullptr };
	};

	FindData data{};
	data.fn_getImg = fn_mono_assembly_get_image;
	data.fn_getName = fn_mono_image_get_name;

	auto callback = [](void* assembly, void* udata)
	{
		auto* d = static_cast<FindData*>(udata);
		if (d->image) return;

		_mono::MonoImage* img = d->fn_getImg(static_cast<_mono::MonoAssembly*>(assembly));
		if (!img) return;

		const char* name = d->fn_getName(img);
		if (name && strcmp(name, "Assembly-CSharp") == 0)
			d->image = img;
	};

	fn_mono_assembly_foreach((void*)&callback, &data);
	m_mainImage = data.image;

	return TRUE;
}
