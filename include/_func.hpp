//
// Created by Lukas on 17 Okt. 2026.
//
#pragma once
#include "_types.hpp"


namespace _mono
{
	typedef MonoDomain*		(*fn_mono_get_root_domain_t)				();
	typedef MonoThread*		(*fn_mono_thread_attach_t)					(MonoDomain*);
	typedef MonoImage*		(*fn_mono_assembly_get_image_t) 			(MonoAssembly*);
	typedef const char*		(*fn_mono_image_get_name_t)					(MonoImage*);
	typedef MonoClass*      (*fn_mono_class_from_name_t)                (MonoImage*, const char*, const char*);
	typedef MonoClassField* (*fn_mono_class_get_field_from_name_t)      (MonoClass*, const char*);
	typedef MonoVTable*     (*fn_mono_class_vtable_t)                   (MonoDomain*, MonoClass*);
	typedef void            (*fn_mono_field_static_get_value_t)         (MonoVTable*, MonoClassField*, void*);
	typedef void            (*fn_mono_field_static_set_value_t)         (MonoVTable*, MonoClassField*, void*);
	typedef void            (*fn_mono_field_get_value_t)                (MonoObject*, MonoClassField*, void*);
	typedef void            (*fn_mono_field_set_value_t)                (MonoObject*, MonoClassField*, void*);
	typedef int             (*fn_mono_field_get_offset_t)               (MonoClassField*);
	typedef void            (*fn_mono_assembly_foreach_t)               (void* func, void* user_data);
	typedef void*           (*fn_mono_class_get_static_field_data_t)    (MonoVTable*);
	typedef uintptr_t       (*fn_mono_class_getStaticFieldAddress_t)    (MonoDomain*, MonoClass*);
}