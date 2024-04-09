#include <Main.h>

/**
 * @brief The main entry point for the BrainGenix-NES Neuron Emulation System.
 *
 * @param NumArguments The number of command-line arguments.
 * @param ArgumentValues An array containing the command-line arguments.
 * @return int The exit code of the program.
 */
int main(int argc, char **argv) {

    BG::NES::IO::SonataLoader loader(argv[1], argv[2]);

    return 0;
}
