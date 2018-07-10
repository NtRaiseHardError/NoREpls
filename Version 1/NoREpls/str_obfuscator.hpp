#ifndef STR_OBFUSCATOR_HPP_
#define STR_OBFUSCATOR_HPP_

namespace detail {
	template<std::size_t index>
	struct encryptor {
		static constexpr void encrypt(wchar_t *dest, const wchar_t *str, wchar_t key) {
			dest[index] = str[index] ^ key;

			encryptor<index - 1>::encrypt(dest, str, key);
		}
	};

	template<>
	struct encryptor<0> {
		static constexpr void encrypt(wchar_t *dest, const wchar_t *str, wchar_t key) {
			dest[0] = str[0] ^ key;
		}
	};
};

class cryptor {
	public:
	template<std::size_t S>
	class string_encryptor {
		public:
		constexpr string_encryptor(const wchar_t str[S], wchar_t key) :
			_buffer{}, _decrypted{ false }, _key{ key } {
			detail::encryptor<S - 1>::encrypt(_buffer, str, key);
		}

		const wchar_t *decrypt(void) const {
			if (_decrypted) {
				return _buffer;
			}

			for (auto &c : _buffer) {
				c ^= _key;
			}

			_decrypted = true;

			return _buffer;
		}

		private:
		mutable wchar_t _buffer[S];
		mutable bool _decrypted;
		const wchar_t _key;
	};

	template<std::size_t S>
	static constexpr auto create(const wchar_t(&str)[S]) {
		return string_encryptor<S>{ str, S };
	}
};

#endif // STR_OBFUSCATOR_HPP_