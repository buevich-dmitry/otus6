#pragma once

#include <iostream>
#include <cassert>
#include <chrono>
#include <fstream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

class ResponseHandler {
public:
    virtual void HandleResponse(const std::vector<std::string>& response) = 0;
    virtual ~ResponseHandler() = default;
};

class ChainResponseHandler : public ResponseHandler {
public:
    void AddHandler(std::unique_ptr<ResponseHandler> handler) {
        handlers_.push_back(std::move(handler));
    }

    void HandleResponse(const std::vector<std::string>& response) override {
        for (const auto& handler : handlers_) {
            handler->HandleResponse(response);
        }
    }

private:
    std::vector<std::unique_ptr<ResponseHandler>> handlers_;
};

class AbstractOstreamResponseHandler : public ResponseHandler {
public:
    void HandleResponse(const std::vector<std::string>& response) override {
        auto& out = GetOstream();
        assert(out.good());
        if (response.empty()) {
            return;
        }
        out << "bulk: ";
        bool first = true;
        for (const auto& command : response) {
            if (!first) {
                out << ", ";
            }
            first = false;
            out << command;
        }
        out << std::endl;
    }

protected:
    virtual std::ostream& GetOstream() = 0;
};

class OstreamResponseHandler : public AbstractOstreamResponseHandler {
public:
    explicit OstreamResponseHandler(std::ostream& out) : out_(out) {
    }

protected:
    std::ostream& GetOstream() override {
        return out_;
    }

private:
    std::ostream& out_;
};

class FileResponseHandler : public AbstractOstreamResponseHandler {
public:
    explicit FileResponseHandler(std::string file_name) : file_(std::move(file_name)) {
    }

protected:
    std::ostream& GetOstream() override {
        return file_;
    }

private:
    std::ofstream file_;
};

std::string MakeBulkFileName() {
    return "bulk" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".log";
}

std::unique_ptr<ResponseHandler> GetMainResponseHandler() {
    std::unique_ptr<ChainResponseHandler> result = std::make_unique<ChainResponseHandler>();
    result->AddHandler(std::make_unique<OstreamResponseHandler>(std::cout));
    result->AddHandler(std::make_unique<FileResponseHandler>(MakeBulkFileName()));
    return result;
}

class CommandHandler {
public:
    explicit CommandHandler(size_t max_block_size) : max_block_size_(max_block_size) {
    }

    [[nodiscard]] std::vector<std::string> HandleCommand(const std::string& command) {
        if (command == "{") {
            ++dynamic_block_necting_;
            if (dynamic_block_necting_ == 1) {
                return FlushCommandBlock();
            }
        } else if (command == "}") {
            assert(dynamic_block_necting_ != 0);
            --dynamic_block_necting_;
            if (dynamic_block_necting_ == 0) {
                return FlushCommandBlock();
            }
        } else {
            command_block_.push_back(command);
            if (dynamic_block_necting_ == 0) {
                if (command_block_.size() == max_block_size_) {
                    return FlushCommandBlock();
                }
            }
        }
        return {};
    }

    [[nodiscard]] std::vector<std::string> Stop() {
        if (dynamic_block_necting_ == 0) {
            return FlushCommandBlock();
        }
        return {};
    }

private:
    std::vector<std::string> FlushCommandBlock() {
        const auto result = std::move(command_block_);
        command_block_.clear();
        return result;
    }

    std::vector<std::string> command_block_;
    int dynamic_block_necting_= 0;
    size_t max_block_size_;
};
