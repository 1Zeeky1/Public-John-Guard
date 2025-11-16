#pragma once

#include <string>
#include <vector>
#include <optional>
#include <json.hpp>

struct EmbedField {
    std::string name;
    std::string value;
    bool is_inline = false;
};

struct EmbedFooter {
    std::string text;
    std::optional<std::string> icon_url;
};

struct EmbedAuthor {
    std::string name;
    std::optional<std::string> url;
    std::optional<std::string> icon_url;
};

struct Embed {
    std::optional<std::string> title;
    std::optional<std::string> description;
    std::optional<std::string> url;
    std::optional<int> color; 
    std::vector<EmbedField> fields;
    std::optional<EmbedFooter> footer;
    std::optional<EmbedAuthor> author;
    std::optional<std::string> thumbnail_url;
    std::optional<std::string> image_url;
    std::optional<std::string> timestamp; 
};

struct WebhookPayload {
    std::string content;
    std::string username;
    std::string avatar_url;
    std::vector<Embed> embeds;

    std::string toJson() const {
        nlohmann::json j;
        j["content"] = content;
        if (!username.empty()) j["username"] = username;
        if (!avatar_url.empty()) j["avatar_url"] = avatar_url;

        if (!embeds.empty()) {
            j["embeds"] = nlohmann::json::array();
            for (const auto& embed : embeds) {
                nlohmann::json e;
                if (embed.title) e["title"] = *embed.title;
                if (embed.description) e["description"] = *embed.description;
                if (embed.url) e["url"] = *embed.url;
                if (embed.color) e["color"] = *embed.color;
                if (embed.timestamp) e["timestamp"] = *embed.timestamp;

                if (!embed.fields.empty()) {
                    e["fields"] = nlohmann::json::array();
                    for (const auto& f : embed.fields) {
                        e["fields"].push_back({
                            {"name", f.name},
                            {"value", f.value},
                            {"inline", f.is_inline}
                        });
                    }
                }

                if (embed.footer) {
                    e["footer"] = { {"text", embed.footer->text} };
                    if (embed.footer->icon_url)
                        e["footer"]["icon_url"] = *embed.footer->icon_url;
                }

                if (embed.author) {
                    e["author"] = { {"name", embed.author->name} };
                    if (embed.author->url)
                        e["author"]["url"] = *embed.author->url;
                    if (embed.author->icon_url)
                        e["author"]["icon_url"] = *embed.author->icon_url;
                }

                if (embed.thumbnail_url)
                    e["thumbnail"] = { {"url", *embed.thumbnail_url} };

                if (embed.image_url)
                    e["image"] = { {"url", *embed.image_url} };

                j["embeds"].push_back(e);
            }
        }

        return j.dump();
    }
};

bool sendWebhook(const std::string& url, const std::string& jsonPayload);