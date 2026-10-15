#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    uint32_t low = (uint32_t)challenge;
    uint32_t high = (uint32_t)(challenge >> 32);
    uint32_t mid = (uint32_t)(challenge >> 16);

    uint32_t t2 = (uint32_t)(5u * high + low) ^ 0x6d13u;
    uint32_t t0 = (uint32_t)(low + 8u * mid) ^ 0xe797u;
    values[0] = (uint32_t)(t2 + 0xc884) ^ t0;
    count = 1;
    return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    uint32_t part0 = (uint32_t)(challenge >> 40);
    values[0] = ((part0 * 2u) & 0x1eu) + 2u;

    for (uint32_t i = 1; i < 6; ++i) {
        uint32_t part = (uint32_t)(challenge >> (i + 18)) & 31u;
        uint32_t previous = values[i - 1];
        uint32_t step = 6u * i;

        values[i] = (uint32_t)(part + previous) ^ step  ;
    }

    count = 6;
    return write_answer(out, cap, values, count);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    uint32_t selector = (uint32_t)challenge & 7u;
    uint32_t x = (uint32_t)(challenge >> 32) ^ (uint32_t)(challenge >> 3);
    uint32_t result;

    values[0] = selector;
    switch (selector) {
    case 0:
        result = 5u * x + 0x436deu;
        break;
    case 1:
        result = (x ^ 0xfc30u) + (x >> 2);
        break;
    case 2:
        result = 5u * x + 0xd48au;
        break;
    case 3:
        result = 5u * x + 0xc723u;
        break;
    case 4:
        result = 5u * x + 0x5b36u;
        break;
    case 5:
        result = (2u * x) ^ 0x8d1fu;
        break;
    case 6:
        result = (x ^ 0x113fu) + (x >> 1);
        break;
    case 7:
        result = (2u * x) ^ 0x11f9u;
        break;
    default:
        return 1;
    }

    values[1] = result;
    count = 2;
    return write_answer(out, cap, values, count);
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    uint32_t target = (uint32_t)challenge & 0x7f;
    uint32_t salt = (uint32_t)(challenge >> 7) ^ (uint32_t)(challenge >> 32);
    uint32_t low = 0;
    uint32_t high = 127;
    uint32_t branch_values[8];
    uint32_t return_masks[8];
    size_t depth = 0;
    uint32_t result;
    for (;;) {
        uint32_t mid = low + (high - low) / 2u;
        if (target == mid) {
            result = (mid + 0x31u) ^ salt;
            break;
        }

        branch_values[depth] = mid + salt;
        if (target < mid) {
            return_masks[depth] = 0x0cu;
            high = mid - 1u;
        } else {
            return_masks[depth] = 0x15u;
            low = mid + 1u;
        }
        ++depth;
    }
    while (depth > 0) {
        --depth;
        result = ((3u * result) ^ branch_values[depth]) ^ return_masks[depth];
    }

    values[0] = result;
    count = 1;
    return write_answer(out, cap, values, count);
}

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    static const uint8_t next_node[] = {
        4, 9, 6, 1, 3, 2, 0, UINT8_MAX, 7, 11, 5, 8
    };
    static const uint32_t node_value[] = {
        0x0263u, 0x0827u, 0x0035u, 0x0a6du,
        0x0753u, 0x0668u, 0x0172u, 0x0b68u,
        0x047bu, 0x0332u, 0x0920u, 0x056bu
    };
    static const uint32_t node_tag[] = {
        0xc940u, 0x9ac4u, 0x782au, 0x713bu,
        0x9eafu, 0x72aau, 0x9c69u, 0x8fdeu,
        0x80d7u, 0x7c46u, 0x9dcau, 0x2d41u
    };
    static const uint8_t heads[] = {10, 5, 2, 6, 0, 4, 3, 1};

    uint32_t node = heads[(uint32_t)challenge & 7u];
    uint32_t hops = (uint32_t)(challenge >> 3) & 3u;
    uint32_t result = ((uint32_t)(challenge >> 6) ^
                       (uint32_t)(challenge >> 32) ^
                       node_tag[node] ^ 0x9843u) + node_value[node];
    uint32_t last_value = node_value[node];

    for (uint32_t i = 0; i < hops; ++i) {
        node = next_node[node];
        if (node >= sizeof(node_value) / sizeof(node_value[0])) return 1;
        result = (result ^ node_tag[node]) + node_value[node];
        last_value = node_value[node];
    }

    values[0] = result;
    values[1] = last_value;
    count = 2;
    return write_answer(out, cap, values, count);
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    uint32_t x = (uint32_t)(challenge >> 6) ^ (uint32_t)(challenge >> 32);
    uint32_t first_index = (uint32_t)challenge & 7u;
    uint32_t second_index = (uint32_t)(challenge >> 3) & 7u;

    static const uint8_t op_for_index[8] = {
        2, 2, 0, 4, 5, 3, 1, 3
    };

    uint32_t selected[2] = {
        op_for_index[first_index],
        op_for_index[second_index]
    };

    for (unsigned stage = 0; stage < 2; ++stage) {
        if (stage == 1) {
            x ^= 0x214eu;
        }

        switch (selected[stage]) {
        case 0:
            x = (x >> 7) | (x << 25);
            break;
        case 1:
            x = (x ^ 0x906bu) + (x << 7);
            break;
        case 2:
            x = ((x >> 1) ^ x) + 0x56e9u;
            break;
        case 3:
            x = (4u * x + 0x6079u) ^ (3u * x);
            break;
        case 4:
            x = (x + 0x4228u) ^ (x >> 1);
            break;
        case 5:
            x = ((x << 1) | (x >> 31)) ^ 0x74cdu;
            break;
        default:
            return 1;
        }
    }

    x += 0x3c60u;
    values[0] = x;
    count = 1;
    return write_answer(out, cap, values, count);
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    uint32_t challenge_fold = (uint32_t)challenge ^ (uint32_t)(challenge >> 32);
    uint32_t context_fold = (uint32_t)context ^ (uint32_t)(context >> 32);
    uint32_t first = (uint32_t)context & 3u;

    uint32_t mix_input = challenge_fold ^ context_fold;
    uint32_t mixed = (mix_input << 3) | (mix_input >> 29);
    mixed = (mixed + 0x9555u) ^ 0xd26du;

    uint32_t second_salt = (0x9555u * first) ^ (uint32_t)(context >> 16);
    uint32_t second = mixed ^ second_salt;
    uint32_t third_tail = context_fold + first + 0xd26du + (challenge_fold >> 3);
    uint32_t third = second ^ third_tail;

    int written = snprintf(out, cap, "%032uOPEN %" PRIu32 " %" PRIu32 " %" PRIu32,
                           0u, first, second, third);
    if (written < 0 || (size_t)written >= cap) return 1;
    return 0;
}
