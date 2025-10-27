#pragma once
#include <random>
#include <utility>
#include <iostream>

// pair hash is needed to make pairs work in unorderd pairs within the NEAT RUNNER Class
struct pair_hash
{
    std::size_t operator()(const std::pair<int, int> &p) const noexcept
    {
        // Simple mixing function (XOR with a shift)
        return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
    }
};

// Global RNG state - using inline for header-only implementation
class RNGManager
{
private:
    static std::mt19937 &getRNGInstance()
    {
        static std::mt19937 rng;
        return rng;
    }

    static bool &getSeededFlag()
    {
        static bool seeded = false;
        return seeded;
    }

public:
    // Seed the RNG (call once at program start)
    static void seed(unsigned int seed_value)
    {
        getRNGInstance().seed(seed_value);
        getSeededFlag() = true;
        std::cout << "[RNG] Seeded with: " << seed_value << "\n";
    }

    // Seed randomly from system clock (for non-deterministic runs)
    static void seedRandom()
    {
        unsigned int seed_value = std::random_device{}();
        getRNGInstance().seed(seed_value);
        getSeededFlag() = true;
        std::cout << "[RNG] Seeded randomly with: " << seed_value << "\n";
    }

    // Get the RNG engine
    static std::mt19937 &getRNG()
    {
        if (!getSeededFlag())
        {
            std::cerr << "⚠️  WARNING: RNG used without seeding! Call RNGManager::seed() or RNGManager::seedRandom()\n";
            seedRandom();
        }
        return getRNGInstance();
    }

    // Get current seed state (for debugging)
    static void printState()
    {
        std::cout << "[RNG] State: seeded=" << (getSeededFlag() ? "yes" : "no") << "\n";
    }
};

// Get random number in range [lower, upper)
inline double getRandNum(double lower, double upper)
{
    std::uniform_real_distribution<double> dist(lower, upper);
    return dist(RNGManager::getRNG());
}

// For backwards compatibility - calls the manager version
inline std::mt19937 &getRNG()
{
    return RNGManager::getRNG();
}