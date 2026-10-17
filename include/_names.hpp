//
// Created by Lukas on 17 Okt. 2026.
//
#pragma once
#include <string_view>
#include <unordered_map>

namespace _mono
{
	inline const std::unordered_map<std::string_view, std::string_view> mono_names
	{
		{"MONO_1", "mono.dll"				},
		{"MONO_2", "mono-2.0-bdwgc.dll"		},
		{"MONO_3", "mono-2.0-sgen.dll"		},
		{"MONO_4", "mono-2.0.dll"			},
		{"MONO_5", "libmono-2.0.dll"		},
		{"MONO_6", "libmonosgen-2.0.dll"	},
		{"MONO_7", "libmonoboehm-2.0.dll"	},
	};
}
