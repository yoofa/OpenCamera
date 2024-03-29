# dependencies file

gclient_gn_args_file = 'src/build/config/gclient_args.gni'
gclient_gn_args = [
  'generate_location_tags',
]

vars = {
  # By default, we should check out everything needed to run on the main
  # chromium waterfalls. More info at: crbug.com/570091.
  'checkout_configuration': 'default',
  'checkout_instrumented_libraries': 'checkout_linux and checkout_configuration == "default"',
  'chromium_revision': '4559b6b576fc5bd8f36ad7cde13bcf5215bec9dc',

  # Keep the Chromium default of generating location tags.
  'generate_location_tags': True,

  # reclient CIPD package version
  'reclient_version': 're_client_version:0.113.0.8b45b89-gomaip',

  # ninja CIPD package version
  # https://chrome-infra-packages.appspot.com/p/infra/3pp/tools/ninja
  'ninja_version': 'version:2@1.11.1.chromium.6',
}

deps = {
  # avp module
  'src/base':
    'https://github.com:yoofa/base.git@3f69082763375f50f401a19416cb3945ba21311b',
  'src/common':
    'https://github.com:yoofa/media_common.git@28f186f35d78a75a32069cfe03bb5cecf1ec3c4c',

  # my third_party
  # inih
  'src/third_party/inih/src':
    'https://github.com/jtilly/inih.git@4a6fde9dd6e1bc129ed307a8cad96bebdd090c80',

  # RtspServer
  'src/third_party/rtsp_server/src':
    'https://github.com/PHZ76/RtspServer.git@1fe0ac9a0a9d417d09c2125b6fd2c1627beea186',

  # openssl
  'src/third_party/openssl':
    'https://github.com:yoofa/third_party_openssl.git@cebc6ad07de0de260d41771692b216574c61d677',

  # gsoap
  'src/third_party/gsoap/src':
    'https://github.com:yoofa/gsoap.git@0a6ee6c2a69c5f8c50cc390e2253a9a2154bfd08',

  # onvif_srvd
  'src/third_party/onvif_srvd/src':
    'https://github.com:yoofa/onvif_srvd.git@9c8c9bdad2438129a592440c9ada620372dc3c39',

  # chromium 
  'src/build':
    'https://chromium.googlesource.com/chromium/src/build@bc10f9ffb962f14c3ed18a6cd9c2f5114d9b0b59',
  'src/buildtools':
    'https://chromium.googlesource.com/chromium/src/buildtools@50c348906cbd450e031bc3123b657f833f8455b7',
  'src/buildtools/linux64': {
    'packages': [
      {
        'package': 'gn/gn/linux-${{arch}}',
        'version': 'git_revision:991530ce394efb58fcd848195469022fa17ae126',
      }
    ],
    'dep_type': 'cipd',
    'condition': 'checkout_linux',
  },
  'src/buildtools/mac': {
    'packages': [
      {
        'package': 'gn/gn/mac-${{arch}}',
        'version': 'git_revision:991530ce394efb58fcd848195469022fa17ae126',
      }
    ],
    'dep_type': 'cipd',
    'condition': 'checkout_mac',
  },
  'src/buildtools/win': {
    'packages': [
      {
        'package': 'gn/gn/windows-amd64',
        'version': 'git_revision:991530ce394efb58fcd848195469022fa17ae126',
      }
    ],
    'dep_type': 'cipd',
    'condition': 'checkout_win',
  },
  'src/buildtools/reclient': {
    'packages': [
      {
         # https://chrome-infra-packages.appspot.com/p/infra/rbe/client/
        'package': 'infra/rbe/client/${{platform}}',
        'version': Var('reclient_version'),
      }
    ],
    'dep_type': 'cipd',
    # Reclient doesn't have linux-arm64 package.
    'condition': 'not (host_os == "linux" and host_cpu == "arm64")',
  },

  'src/third_party/clang-format/script':
    'https://chromium.googlesource.com/external/github.com/llvm/llvm-project/clang/tools/clang-format.git@e5337933f2951cacd3aeacd238ce4578163ca0b9',
  'src/third_party/libc++/src':
    'https://chromium.googlesource.com/external/github.com/llvm/llvm-project/libcxx.git@7cf98622abaf832e2d4784889ebc69d5b6fde4d8',
  'src/third_party/libc++abi/src':
    'https://chromium.googlesource.com/external/github.com/llvm/llvm-project/libcxxabi.git@e8e4eb8f1c413ea4365256b2b83a6093c95d2d86',
  'src/third_party/libunwind/src':
    'https://chromium.googlesource.com/external/github.com/llvm/llvm-project/libunwind.git@43e5a34c5b7066a7ee15c74f09dc37b4b9b5630e',

  'src/third_party/ninja': {
    'packages': [
      {
        'package': 'infra/3pp/tools/ninja/${{platform}}',
        'version': Var('ninja_version'),
      }
    ],
    'dep_type': 'cipd',
  },

  'src/testing':
    'https://chromium.googlesource.com/chromium/src/testing@074ff55a07fb690e769eda906375f8e7cb6b39b8',
  'src/third_party/depot_tools':
    'https://chromium.googlesource.com/chromium/tools/depot_tools.git@90a30a5b5357636fa05bb315c393275be7ca705c',

  'src/third_party/googletest/src':
    'https://chromium.googlesource.com/external/github.com/google/googletest.git@af29db7ec28d6df1c7f0f745186884091e602e07',

  'src/third_party/libyuv':
    'https://chromium.googlesource.com/libyuv/libyuv.git@04821d1e7d60845525e8db55c7bcd41ef5be9406',
  'src/third_party/nasm': {
      'url': 'https://chromium.googlesource.com/chromium/deps/nasm.git@7fc833e889d1afda72c06220e5bed8fb43b2e5ce'
  },

  'src/third_party/openh264/src':
    'https://chromium.googlesource.com/external/github.com/cisco/openh264@3dd5b80bc4f172dd82925bb259cb7c82348409c5',

  'src/third_party/fdk-aac/src':
    'https://github.com/mstorsjo/fdk-aac.git@3f864cce9736cc8e9312835465fae18428d76295',

  'src/tools/clang/dsymutil': {
    'packages': [
      {
        'package': 'chromium/llvm-build-tools/dsymutil',
        'version': 'M56jPzDv1620Rnm__jTMYS62Zi8rxHVq7yw0qeBFEgkC',
      }
    ],
    'condition': 'checkout_mac',
    'dep_type': 'cipd',
  },

  # third_party
  'src/third_party/catapult':
    'https://chromium.googlesource.com/catapult.git@4f81c1e295978227d83f1b42ceff40b4f9b5b08c',

  'src/tools':
    'https://chromium.googlesource.com/chromium/src/tools@33a950a48cf447a6b1c0262e9955446e6d129d34',
  'src/tools/swarming_client':
    'https://chromium.googlesource.com/infra/luci/client-py.git@d46ea7635f2911208268170512cb611412488fd8',
}

hooks = [
  {
    # Ensure that the DEPS'd "depot_tools" has its self-update capability
    # disabled.
    'name': 'disable_depot_tools_selfupdate',
    'pattern': '.',
    'action': [
        'python',
        'src/third_party/depot_tools/update_depot_tools_toggle.py',
        '--disable',
    ],
  },
  {
    'name': 'sysroot_arm',
    'pattern': '.',
    'condition': 'checkout_linux and checkout_arm',
    'action': ['python', 'src/build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=arm'],
  },
  {
    'name': 'sysroot_arm64',
    'pattern': '.',
    'condition': 'checkout_linux and checkout_arm64',
    'action': ['python', 'src/build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=arm64'],
  },
  {
    'name': 'sysroot_x86',
    'pattern': '.',
    'condition': 'checkout_linux and (checkout_x86 or checkout_x64)',
    # TODO(mbonadei): change to --arch=x86.
    'action': ['python', 'src/build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=i386'],
  },
  {
    'name': 'sysroot_mips',
    'pattern': '.',
    'condition': 'checkout_linux and checkout_mips',
    # TODO(mbonadei): change to --arch=mips.
    'action': ['python', 'src/build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=mipsel'],
  },
  {
    'name': 'sysroot_x64',
    'pattern': '.',
    'condition': 'checkout_linux and checkout_x64',
    # TODO(mbonadei): change to --arch=x64.
    'action': ['python', 'src/build/linux/sysroot_scripts/install-sysroot.py',
               '--arch=amd64'],
  },
  {
    # Case-insensitivity for the Win SDK. Must run before win_toolchain below.
    'name': 'ciopfs_linux',
    'pattern': '.',
    'condition': 'checkout_win and host_os == "linux"',
    'action': [ 'python',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-browser-clang/ciopfs',
                '-s', 'src/build/ciopfs.sha1',
    ]
  },
  {
    # Update the Windows toolchain if necessary. Must run before 'clang' below.
    'name': 'win_toolchain',
    'pattern': '.',
    'condition': 'checkout_win',
    'action': ['python', 'src/build/vs_toolchain.py', 'update', '--force'],
  },
  {
    # Update the Mac toolchain if necessary.
    'name': 'mac_toolchain',
    'pattern': '.',
    'condition': 'checkout_mac',
    'action': ['python', 'src/build/mac_toolchain.py'],
  },
  {
    # Note: On Win, this should run after win_toolchain, as it may use it.
    'name': 'clang',
    'pattern': '.',
    'action': ['python', 'src/tools/clang/scripts/update.py'],
  },
  # Pull clang-format binaries using checked-in hashes.
  {
    'name': 'clang_format_win',
    'pattern': '.',
    'condition': 'host_os == "win"',
    'action': [ 'download_from_google_storage',
                '--no_resume',
                '--platform=win32',
                '--no_auth',
                '--bucket', 'chromium-clang-format',
                '-s', 'src/buildtools/win/clang-format.exe.sha1',
    ],
  },
  {
    'name': 'clang_format_mac',
    'pattern': '.',
    'condition': 'host_os == "mac"',
    'action': [ 'download_from_google_storage',
                '--no_resume',
                '--platform=darwin',
                '--no_auth',
                '--bucket', 'chromium-clang-format',
                '-s', 'src/buildtools/mac/clang-format.sha1',
    ],
  },
  {
    'name': 'clang_format_linux',
    'pattern': '.',
    'condition': 'host_os == "linux"',
    'action': [ 'download_from_google_storage',
                '--no_resume',
                '--platform=linux*',
                '--no_auth',
                '--bucket', 'chromium-clang-format',
                '-s', 'src/buildtools/linux64/clang-format.sha1',
    ],
  },
  {                                                                                                 
    # Update LASTCHANGE.                                                                            
    'name': 'lastchange',                                                                           
    'pattern': '.',                                                                                 
    'action': ['python', 'src/build/util/lastchange.py',                                            
               '-o', 'src/build/util/LASTCHANGE'],                                                  
  },
]
