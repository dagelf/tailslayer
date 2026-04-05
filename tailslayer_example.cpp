#include <tailslayer/hedged_reader.hpp>
#include <iostream>

// void dumb_wait(int arg1, int arg2) {
//     std::cout << "Hi\n";
// }

// template <typename T>
// void dumb_work(T val, int arg2) {
//     std::cout << "Hi\n";
// }

/*
Example user functions that could be passed to tailslayer
*/
[[gnu::always_inline]] inline std::size_t dummy_read_signal() {
    uint64_t starting_time_dumb = tailslayer::detail::rdtsc_lfence();
    tailslayer::detail::rdtsc_lfence();
    uint64_t num_cycles{0};
    do {
        num_cycles = tailslayer::detail::rdtsc_lfence() - starting_time_dumb;
    } while (num_cycles < 2000000000);

    std::size_t dummy_offset = 1; // Let's read the second value

    return dummy_offset;
}

template <typename T>
[[gnu::always_inline]] inline void dummy_final_work(T val) {
    asm volatile("" :: "r"(val)); // Dummy using value
    std::cout << "Val: " << val << "\n";
}

int main() {
    using target_size_t = uint8_t;
    tailslayer::pin_to_core(tailslayer::CORE_MAIN);

    std::cout << "Start tailslayer demo.\n";

    // std::cout << "Normal dumb read:\n";
    // uint64_t t0, t1;
    // volatile target_size_t normal_val = 0x43;
    // for (int i = 0; i < 10; i++) {
    //     tailslayer::detail::clflush_addr(&normal_val);
    //     tailslayer::detail::mfence_inst();
    //     t0 = tailslayer::detail::rdtsc_lfence();
    //     target_size_t val = normal_val;
    //     asm volatile("" :: "r"(val));
    //     t1 = tailslayer::detail::rdtscp_lfence();
    //     std::cout << "t0: " << t0 << "\n";
    //     std::cout << "t1: " << t1 << "\n";
    //     std::cout << "Time: " << t1 - t0 << " cycles\n";
    // }
    // std::cout << "Done with normal dumb read\n";

    // Example with args:
    // using WaitArgs = tailslayer::ArgList<1, 2>;
    // using WorkArgs = tailslayer::ArgList<2>;
    // tailslayer::HedgedReader<target_size_t, dumb_wait, dumb_work<target_size_t>, WaitArgs, WorkArgs> reader_args{};

    tailslayer::HedgedReader<target_size_t, dummy_read_signal, dummy_final_work<target_size_t>> reader{};
    reader.insert(0x43);
    reader.insert(0x44);
    reader.start_workers();

    std::cout << "End tailslayer demo.\n";

    return 0;
}