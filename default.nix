{ pkgs ? import <nixpkgs> {}, ...}: pkgs.stdenv.mkDerivation {
    name = "test";
    src = ./.;
    nativeBuildInputs = [
        pkgs.cmake
        pkgs.nlohmann_json
    ];
    configurePhase = ''
        cmake .
    '';
    buildPhase = ''
        make -j
    '';
    installPhase = ''
        make install
    '';
    # buildPhase = ''
    #     echo "----------------"
    #     ls ..
    #     echo "----------------"
    #     mkdir build
    #     cd build
    #     cmake ..
    #     mkdir -p $out/bin
    #     mv Test $out/bin/
    # '';
}