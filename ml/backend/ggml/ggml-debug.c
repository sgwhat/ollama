#include <string.h>

#include "ggml-debug.h"

static int mul(int64_t *dims, int ndims) {
    int result = 1;
    for (int i = 0; i < ndims; i++) {
        result *= dims[i];
    }

    return result;
}

static void repeat(char c, int n) {
    for (int i = 0; i < n; i++) {
        fprintf(stderr, "%c", c);
    }
}

static void print_tensor(const void *tensor, void (*cb)(const void *, int),
                         int shape,
                         int64_t *dims, int ndims, int stride,
                         int nitems, int pad) {
    fprintf(stderr, "[");
    for (int i = 0; i < dims[0]; i++) {
        if (i >= nitems && i < dims[0] - nitems) {
            fprintf(stderr, "... (%lld more), ", dims[0] - 2 * nitems);
            int skip = dims[0] - 2 * nitems;
            if (ndims > 1) {
                stride += mul(dims + 1, ndims - 1) * skip;
                repeat('\n', ndims - 1);
                repeat(' ', shape - ndims + 1 + pad);
            }
            i += skip - 1;
        } else if (ndims > 1) {
            print_tensor(tensor, cb, shape, dims + 1, ndims - 1, stride,
                         nitems, pad);
            stride += mul(dims + 1, ndims - 1);
            if (i < dims[0] - 1) {
                fprintf(stderr, ", ");
                repeat('\n', ndims - 1);
                repeat(' ', shape - ndims + 1 + pad);
            }
        } else {
            cb(tensor, stride + i);
            if (i < dims[0] - 1) {
                fprintf(stderr, ", ");
            }
        }
    }
    fprintf(stderr, "]");
}

static void print_tensor_f16(const void *tensor, int i) {
    fprintf(stderr, "%f", ggml_fp16_to_fp32(((const ggml_fp16_t *)tensor)[i]));
}

static void print_tensor_f32(const void *tensor, int i) {
    fprintf(stderr, "%f", ((const float *)tensor)[i]);
}

static void print_tensor_i32(const void *tensor, int i) {
    fprintf(stderr, "%d", ((const int32_t *)tensor)[i]);
}

static void ggml_debug_tensor(const struct ggml_tensor *tensor, bool verbose, const char *prefix, int indent) {
    fprintf(stderr, "%s%s %s (%s): [%lld %lld %lld %lld]\n", prefix, tensor->name,
            ggml_op_name(tensor->op), ggml_type_name(tensor->type), tensor->ne[0],
            tensor->ne[1], tensor->ne[2], tensor->ne[3]);

    if (!verbose) {
        return;
    }

    for (int i = 0; i < indent; i++) {
        fprintf(stderr, " ");
    }

    switch (tensor->type) {
    case GGML_TYPE_F16:
        print_tensor(ggml_get_data(tensor), print_tensor_f16, ggml_n_dims(tensor),
                     (int64_t *)tensor->ne, ggml_n_dims(tensor), 0, 3, indent);
        break;
    case GGML_TYPE_F32:
        print_tensor(ggml_get_data(tensor), print_tensor_f32, ggml_n_dims(tensor),
                     (int64_t *)tensor->ne, ggml_n_dims(tensor), 0, 3, indent);
        break;
    case GGML_TYPE_I32:
        print_tensor(ggml_get_data(tensor), print_tensor_i32, ggml_n_dims(tensor),
                     (int64_t *)tensor->ne, ggml_n_dims(tensor), 0, 3, indent);
        break;
    default:
        fprintf(stderr, "<unsupported type>\n");
        return;
    }

    fprintf(stderr, "\n");
}

void ggml_debug(const struct ggml_tensor *tensor, bool verbose) {
    ggml_debug_tensor(tensor, verbose, ">>> ", 4);

    if (tensor->src[0] != NULL) {
        ggml_debug_tensor(tensor->src[0], verbose, " ?? ", 4);
    }

    if (tensor->src[1] != NULL) {
        ggml_debug_tensor(tensor->src[1], verbose, " ?? ", 4);
    }
}
