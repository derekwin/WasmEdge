// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2019-2022 Second State INC

#pragma once

#include "plugin/plugin.h"
#include "types.h"

#ifdef WASMEDGE_PLUGIN_WASI_NN_BACKEND_GGML
#include <common.h>
#include <llama.h>
#endif

namespace WasmEdge::Host::WASINN {
struct WasiNNEnvironment;
}

namespace WasmEdge::Host::WASINN::GGML {

#ifdef WASMEDGE_PLUGIN_WASI_NN_BACKEND_GGML
struct Graph {
  llama_model *LlamaModel = nullptr;
  std::string ModelFilePath;
  // Plugin parameters:
  bool EnableLog;
  bool EnableDebugLog;
  bool StreamStdout;
  uint64_t NPredict;
  std::string ReversePrompt;
  // Model parameters:
  int64_t NGPULayers;
  // Context parameters:
  uint64_t CtxSize;
  uint64_t BatchSize;
  uint64_t Threads;
  // Sampleing parameters:
  double Temp;
  double RepeatPenalty;
};

struct Context {
public:
  Context(size_t GId, Graph &) noexcept : GraphId(GId) {}
  size_t GraphId;
  std::vector<llama_token> LlamaInputs;
  std::string LlamaOutputs;
  std::vector<llama_token> LlamaOutputTokens;
  // Preserve for computing single token
  llama_context *LlamaContext = nullptr;
  struct llama_sampling_context *LlamaSampling = nullptr;
  std::vector<llama_token> LlamaEmbd;
  uint64_t LlamaNPast;
  uint64_t LlamaNConsumed;
};
#else
struct Graph {};
struct Context {
  Context(size_t, Graph &) noexcept {}
};
#endif

struct Environ {};

Expect<WASINN::ErrNo> load(WASINN::WasiNNEnvironment &Env,
                           Span<const Span<uint8_t>> Builders,
                           WASINN::Device Device, uint32_t &GraphId) noexcept;
Expect<WASINN::ErrNo> initExecCtx(WASINN::WasiNNEnvironment &Env,
                                  uint32_t GraphId,
                                  uint32_t &ContextId) noexcept;
Expect<WASINN::ErrNo> setInput(WASINN::WasiNNEnvironment &Env,
                               uint32_t ContextId, uint32_t Index,
                               const TensorData &Tensor) noexcept;
Expect<WASINN::ErrNo> getOutput(WASINN::WasiNNEnvironment &Env,
                                uint32_t ContextId, uint32_t Index,
                                Span<uint8_t> OutBuffer,
                                uint32_t &BytesWritten) noexcept;
Expect<WASINN::ErrNo> getOutputSingle(WASINN::WasiNNEnvironment &Env,
                                      uint32_t ContextId, uint32_t Index,
                                      Span<uint8_t> OutBuffer,
                                      uint32_t &BytesWritten) noexcept;
Expect<WASINN::ErrNo> compute(WASINN::WasiNNEnvironment &Env,
                              uint32_t ContextId) noexcept;
Expect<WASINN::ErrNo> computeSingle(WASINN::WasiNNEnvironment &Env,
                                    uint32_t ContextId) noexcept;
Expect<WASINN::ErrNo> finiSingle(WASINN::WasiNNEnvironment &Env,
                                 uint32_t ContextId) noexcept;
} // namespace WasmEdge::Host::WASINN::GGML
