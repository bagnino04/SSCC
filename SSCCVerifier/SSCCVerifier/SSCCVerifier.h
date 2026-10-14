#pragma once

// Structure to store information about GS1 Application Identifiers
struct AIConfig {
    std::string ai;             //AI identifier
    size_t length;              //Payload length
    std::string description;    // User friendly description for user notification
};

const std::vector<AIConfig> GS1_AI_DATABASE = {
    {"00", 18, "SSCC (Serial Shipping Container Code)"},
    // Placeholder for other AIs which support may be added in the future
};