//
// Created by Lukas on 17 Okt. 2026.
//
#pragma once
#include <Windows.h>
#include <string>
#include <iostream>

#include "_func.hpp"


class MonoAPI {

public:

	// -+-+-+-+-+-+-+- Constructor -+-+-+-+-+-+-+-

	explicit MonoAPI(const std::string& mono_dll_name = "");

	// -+-+-+-+-+-+-+- Main Functions -+-+-+-+-+-+-+-

	BOOL Start();

	_mono::MonoClass* FindClass(const std::string& _class, const std::string& _namespace);
	_mono::MonoVTable* GetVTable(_mono::MonoClass* _class);

	uintptr_t GetStaticPointer(_mono::MonoClass* _class, const std::string& fieldName);
	uintptr_t GetStaticBase(_mono::MonoClass* _class);

	template<typename T>
	BOOL GetStaticField(_mono::MonoClass* _class, const std::string& fieldName, T& outValue)
	{
		if (!m_initialized)
		{
			std::cerr << "[ MONO ] Failed to get static field with mono not initialized!\n";
			return FALSE;
		}

		_mono::MonoClassField* field =
			fn_mono_class_get_field_from_name(_class, fieldName.c_str());

		if (!field) {
			std::cerr << "[ MONO ] Couldn't find static field with name: " << fieldName << '\n';
			return FALSE;
		}

		_mono::MonoVTable* vTable = GetVTable(_class);
		if (!vTable) {
			std::cerr << "[ MONO ] Failed to get VTable in ( GetStaticField )\n";
			return FALSE;
		}

		fn_mono_field_static_get_value(vTable, field, &outValue);
		return outValue != 0;
	}

	template<typename T>
	BOOL SetStaticField(_mono::MonoClass* _class, const std::string& fieldName, T value)
	{
		if (!m_initialized)
		{
			std::cerr << "[ MONO ] Failed to set static base with mono not initialized!\n";
			return FALSE;
		}

		_mono::MonoClassField* field = fn_mono_class_get_field_from_name(_class, fieldName.c_str());
		if (!field) {
			std::cerr << "[ MONO ] Couldn't find static field with name: " << fieldName << '\n';
			return FALSE;
		}

		_mono::MonoVTable* vTable = GetVTable(_class);
		if (!vTable) {
			std::cerr << "[ MONO ] Failed to get VTable in ( SetStaticField )\n";
			return FALSE;
		}

		fn_mono_field_static_set_value(vTable, field, &value);
		return TRUE;
	}

	// -+-+-+-+-+-+-+- Class Getter -+-+-+-+-+-+-+-

	[[nodiscard]]
	const BOOL& IsInitialized() const noexcept {
		return m_initialized != 0;
	}

	[[nodiscard]]
	_mono::MonoDomain* GetDomain() const noexcept {
		return m_domain;
	}

	[[nodiscard]]
	_mono::MonoImage* GetMainImage() const noexcept {
		return m_mainImage;
	}

private:

	BOOL FindMonoDLLName();
	BOOL FindMainImage();

	BOOL m_initialized { FALSE };

	HMODULE m_mono { nullptr };

	std::string m_mono_dll_name{};

	_mono::MonoDomain* m_domain		{ nullptr };
	_mono::MonoImage*  m_mainImage	{ nullptr };

	_mono::fn_mono_get_root_domain_t               fn_mono_get_root_domain             = nullptr;
	_mono::fn_mono_thread_attach_t                 fn_mono_thread_attach               = nullptr;
	_mono::fn_mono_assembly_get_image_t            fn_mono_assembly_get_image          = nullptr;
	_mono::fn_mono_image_get_name_t                fn_mono_image_get_name              = nullptr;
	_mono::fn_mono_class_from_name_t               fn_mono_class_from_name             = nullptr;
	_mono::fn_mono_class_get_field_from_name_t     fn_mono_class_get_field_from_name   = nullptr;
	_mono::fn_mono_class_vtable_t                  fn_mono_class_vtable                = nullptr;
	_mono::fn_mono_field_static_get_value_t        fn_mono_field_static_get_value      = nullptr;
	_mono::fn_mono_field_static_set_value_t        fn_mono_field_static_set_value      = nullptr;
	_mono::fn_mono_field_get_value_t               fn_mono_field_get_value             = nullptr;
	_mono::fn_mono_field_set_value_t               fn_mono_field_set_value             = nullptr;
	_mono::fn_mono_field_get_offset_t              fn_mono_field_get_offset            = nullptr;
	_mono::fn_mono_assembly_foreach_t              fn_mono_assembly_foreach            = nullptr;
	_mono::fn_mono_class_get_static_field_data_t   fn_mono_class_get_static_field_data = nullptr;
	_mono::fn_mono_class_getStaticFieldAddress_t   fn_mono_class_getStaticFieldAddress = nullptr;

};
