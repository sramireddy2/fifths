#include "note.hpp"

#include <iostream>

int main() {
    std::cout << "fifths — real-time instrument tuner\n\n";

    // Sanity check the frequency → note mapping before audio is wired up.
    for (const double hz : {82.41, 110.0, 146.83, 196.0, 246.94, 329.63, 440.0}) {
        std::cout << "  " << format_pitch(frequency_to_pitch(hz)) << '\n';
    }

    std::cout << "\nNext: capture mic input (miniaudio) and detect pitch live.\n";
    return 0;
}
