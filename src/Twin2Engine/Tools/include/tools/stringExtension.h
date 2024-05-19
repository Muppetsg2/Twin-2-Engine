#pragma once

namespace Twin2Engine::Tools {
	
	template<class String, class... Strings> void concat_impl(std::string& out, String&& string, Strings&&... strings) {
		out.append(string);
		if constexpr (sizeof...(strings) > 0) {
			concat_impl(out, std::forward<Strings>(strings)...);
		}
	}

	template<class String, class... Strings> std::string concat(String&& string, Strings&&... strings) {
		std::string out;
		concat_impl(out, std::forward<String>(string), std::forward<Strings>(strings)...);
		return out;
	}

	template<class String, class... Strings> void concat_ref(std::string& out, String&& string, Strings&&... strings) {
		concat_impl(out, std::forward<String>(string), std::forward<Strings>(strings)...);
	}
}