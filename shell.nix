{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = [
    pkgs.gcc
    pkgs.cmake
    (pkgs.opencv.override { enableGtk3 = true; })
    pkgs.fftw
    pkgs.curl
    pkgs.spdlog
    pkgs.eigen
    pkgs.fmt
  ];
}

