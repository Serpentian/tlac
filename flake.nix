{
  description = "TLA trace validation C project (CMake) + TLA+ tools in devShell";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      systems = [ "x86_64-linux" "aarch64-linux" ];
      forAllSystems = f: nixpkgs.lib.genAttrs systems (system: f system);
    in
    {
      packages = forAllSystems (system:
        let
          pkgs = import nixpkgs { inherit system; };

          tlaplusJars = pkgs.stdenvNoCC.mkDerivation {
            pname = "tlaplus-jars";
            version = "1.8.0";

            tla2tools = pkgs.fetchurl {
              url = "https://github.com/tlaplus/tlaplus/releases/download/v1.8.0/tla2tools.jar";
              sha256 = "sha256-OXgpd1xuyvhveunlybBi/N6jnxtp/J8Kmp8PYX3eSZ4=";
            };

            # Latest CommunityModules release at the moment is tag 202601200755
            communityDeps = pkgs.fetchurl {
              url = "https://github.com/tlaplus/CommunityModules/releases/download/202601200755/CommunityModules-deps.jar";
              sha256 = "sha256-W3u22U6hzP3SYRcWuBQwII9wm0k657Tzn6fLhqYCky4=";
            };

            dontUnpack = true;

            installPhase = ''
              set -eu
              mkdir -p "$out/toolbox"
              cp "$tla2tools" "$out/toolbox/tla2tools.jar"
              cp "$communityDeps" "$out/toolbox/CommunityModules-deps.jar"
            '';
          };

        in
        {
          tlaplus-jars = tlaplusJars;

          default = pkgs.stdenv.mkDerivation {
            pname = "tlac";
            version = "0.1.0";
            src = self;

            nativeBuildInputs = [
              pkgs.cmake
              pkgs.ninja
              pkgs.pkg-config
            ];

            buildInputs = [
              pkgs.cjson
            ];

            cmakeFlags = [
              "-DCMAKE_BUILD_TYPE=Release"
            ];
          };
        }
      );

      devShells = forAllSystems (system:
        let
          pkgs = import nixpkgs { inherit system; };
          tlaplusJars = self.packages.${system}.tlaplus-jars;
        in
        {
          default = pkgs.mkShell {
            packages = [
              pkgs.clang
              pkgs.cmake
              pkgs.ninja
              pkgs.pkg-config
              pkgs.cjson
              pkgs.python3
              pkgs.jdk17  # or jdk11; TLC usually ok on 11/17
            ];

            shellHook = ''
              export TLA_DIR="${tlaplusJars}/toolbox"
              echo "TLA_DIR=$TLA_DIR"
              echo "tla2tools.jar: $TLA_DIR/tla2tools.jar"
              echo "CommunityModules-deps.jar: $TLA_DIR/CommunityModules-deps.jar"
            '';
          };
        }
      );
    };
}
