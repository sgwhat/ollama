package main

import (
	"encoding/json"
	"os"

	"ollama/llama/llamafile"
	"ollama/version"
)

func printRequirements(fp *os.File) {
	attrs := map[string]string{
		"system_info":  llama.PrintSystemInfo(),
		"version":      version.Version,
		"cpu_features": llama.CpuFeatures,
	}
	enc := json.NewEncoder(fp)
	_ = enc.Encode(attrs)
}
