project('duplinker', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++17'])

cryptopp_dep = dependency('cryptopp', required:false)

if not cryptopp_dep.found()
  cryptopp_dep = dependency('libcryptopp', required:false)
endif

if not cryptopp_dep.found()
  cryptopp_dep = dependency('libcrypto++', required:false)
endif

executable('duplinker',
           'src/main.cpp',
           install : false,
		   dependencies: [cryptopp_dep])
