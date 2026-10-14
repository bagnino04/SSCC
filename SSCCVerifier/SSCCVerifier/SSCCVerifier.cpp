// SSCCVerifyer.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include <string>
#include <vector>

#include "SSCCVerifier.h"

const std::string COMPANY_ID = "34260311";

// Helper function to calculate GS1 Mod 10 Check Digit
int calculate_gs1_check_digit(const std::string& digits) {
    int sum = 0;
    bool multiply_by_three = true;

    // Process from right to left (excluding the check digit itself)
    for (int i = static_cast<int>(digits.length()) - 1; i >= 0; --i) {
        int digit = digits[i] - '0';
        sum += digit * (multiply_by_three ? 3 : 1);
        multiply_by_three = !multiply_by_three;
    }

    int remainder = sum % 10;
    return (remainder == 0) ? 0 : (10 - remainder);
}

// Validates a single parsed GS1 payload
bool validate_ai_payload(const std::string& ai, const std::string& payload, std::vector<std::string>& errors) {
    bool valid = true;

    // Specific validation rules for SSCC (AI 00)
    if (ai == "00") {
        if (payload.length() != 18) {
            errors.push_back("Error (AI 00): SSCC payload must be exactly 18 digits. Found " + std::to_string(payload.length()) + " digits.");
            return false;
        }
        int expected_check = calculate_gs1_check_digit(payload.substr(0, 17));
        int actual_check = payload.back() - '0';
        if (expected_check != actual_check) {
            errors.push_back("Error (AI 00): Invalid SSCC check digit. Expected '" + std::to_string(expected_check) + "', but found '" + std::to_string(actual_check) + "'.");
            valid = false;
        }
        // Check if company ID is valid:
        if (payload.substr(1, 8) != COMPANY_ID) {
            errors.push_back("Error (AI 00): Invalid Company ID. Expected '" + COMPANY_ID + "', but found '" + payload.substr(1, 8) + "'.");
            valid = false;
		}
    }
    else {
        // Unsupported AI:
        errors.push_back("Error: Unsupported AI!");
        valid = false;
    }

    return valid;
}

// Parses and validates the full barcode string
void parse_gs1_128(const std::string& barcode) {
    std::cout << "\n----------------------------------------\n";
    std::cout << "Analyzing Barcode String: " << barcode << "\n";
    std::cout << "----------------------------------------\n";

    if (barcode.empty()) {
        std::cout << "Error: Barcode contains no numeric digits.\n";
        return;
    }

    size_t index = 0;
    std::vector<std::string> errors;
    std::vector<std::pair<std::string, std::string>> parsed_elements;
    AIConfig matched_ai;

    while (index < barcode.length()) {
        bool ai_found = false;

        // Try to match an AI from our database (checking 2-digit combinations first)
        for (const auto& config : GS1_AI_DATABASE) {
            if (barcode.compare(index, config.ai.length(), config.ai) == 0) {
                matched_ai = config;
                ai_found = true;
                break;
            }
        }

        if (!ai_found) {
            std::string unknown_ai = barcode.substr(index, 2);
            errors.push_back("Error: Undefined or unsupported Application Identifier '" + unknown_ai + "' starting at position " + std::to_string(index + 1) + ".");
            break;
        }

        index += matched_ai.ai.length();

        // Extract payload
        std::string payload;
        if (index + matched_ai.length > barcode.length()) {
            errors.push_back("Error: AI '" + matched_ai.ai + "' (" + matched_ai.description + ") requires " + std::to_string(matched_ai.length) + " digits, but only " + std::to_string(barcode.length() - index) + " digits found!");
            break;
        }
        payload = barcode.substr(index, matched_ai.length);
        index += matched_ai.length;

        parsed_elements.push_back({ matched_ai.ai, payload });
        validate_ai_payload(matched_ai.ai, payload, errors);
    }

    // Output parsing results
    if (errors.empty()) {
        std::cout << "Barcode is CONFORMING to GS1-128 Specifications!\n\n";
        std::cout << "Parsed Structure:\n";
        for (const auto& [ai, payload] : parsed_elements) {
            std::cout << "  AI: " << ai << " Payload: " << payload << "\n";
        }
    }
    else {
        std::cout << "Barcode is NON-CONFORMING.\n\n";
        std::cout << "Validation Details:\n";
        for (const auto& err : errors) {
            std::cout << "  " << err << "\n";
        }
    }
}

void clearConsole() {
    #ifdef _WIN32
        system("cls");   // Windows
    #else
        std::system("clear"); // Linux and macOS
    #endif
}

int main()
{
    std::cout << "GS1-128 Verification Tool\n";
    std::string input;

    while (true) {
		
        std::cout << "\nEnter barcode string (or 'exit' to quit): ";
        std::getline(std::cin, input);

        if (input == "exit" || input == "EXIT") {
            break;
        }
        else {
			// clear the console before processing the next input
            clearConsole();
        }

        int i = 1;
        parse_gs1_128(input);
    }

    return 0;
}