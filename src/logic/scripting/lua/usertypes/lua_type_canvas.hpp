#pragma once

#include "../lua_commons.hpp"

class Texture;
class ImageData;

namespace lua {
    class LuaCanvas : public Userdata {
    public:
        explicit LuaCanvas(
            std::shared_ptr<Texture> inTexture,
            std::shared_ptr<ImageData> inData
        );
        ~LuaCanvas() override = default;

        const std::string& getTypeName() const override {
            return TYPENAME;
        }

        [[nodiscard]] auto& texture() const {
            return *mTexture;
        }

        [[nodiscard]] auto& data() const {
            return *mData;
        }

        [[nodiscard]] bool hasTexture() const {
            return mTexture != nullptr;
        }

        auto shareTexture() const {
            return mTexture;
        }

        void createTexture();

        static int createMetatable(lua::State*);
        inline static std::string TYPENAME = "Canvas";
    private:
        std::shared_ptr<Texture> mTexture; // nullable
        std::shared_ptr<ImageData> mData;
    };
    static_assert(!std::is_abstract<LuaCanvas>());
}
