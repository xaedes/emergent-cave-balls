
#include <catch2/catch.hpp>

#include "transform/version.h"
#include "transform/transform.h"

// this is for an old implementation of Transform, but can be used as inspiration for tests for the new implementation

TEST_CASE("transform::Transform", "[gui_application][Transform]")
{
    using Transform = transform::Transform_<std::string>;
    using TransformPtr = Transform::pointer_type;
    
    TransformPtr root = Transform::CreateWith(std::string("root"));

    REQUIRE(root->localPosition() == glm::vec3(0,0,0));
    REQUIRE(root->localRotation() == glm::mat3(1));

    REQUIRE(root->worldPosition() == glm::vec3(0,0,0));
    REQUIRE(root->worldRotation() == glm::mat3(1));
    
    SECTION("")
    {
        // above root and rotated around upward axis 
        TransformPtr tf = Transform::Create(root, glm::vec3(0,10,0), 0,glm::radians(45.0f),0);
        REQUIRE(tf->worldPosition().x == 0);
        REQUIRE(tf->worldPosition().y == 10);
        REQUIRE(tf->worldPosition().z == 0);

        glm::mat4 mat = tf->transformLocalToRoot();
        glm::vec4 vec = mat * glm::vec4(1,0,1,1);
        REQUIRE(abs(vec.z - 0) < 1e-6f);
        REQUIRE(abs(vec.x - std::sqrt(1*1 + 1*1)) < 1e-6f);
        REQUIRE(abs(vec.y - 10) < 1e-6f);
    }

    SECTION("create with payload")
    {
        TransformPtr a = Transform::CreateWith("payload", root);
        REQUIRE(a->object == "payload");
    }
    SECTION("foreach")
    {
        TransformPtr a = Transform::CreateWith("a", root);
        TransformPtr aa = Transform::CreateWith("aa", a);
        TransformPtr ab = Transform::CreateWith("ab", a);
        TransformPtr aba = Transform::CreateWith("aba", ab);
        TransformPtr abb = Transform::CreateWith("abb", ab);
        TransformPtr abc = Transform::CreateWith("abc", ab);
        TransformPtr abca = Transform::CreateWith("abca", abc);
        TransformPtr abd = Transform::CreateWith("abd", ab);
        TransformPtr ac = Transform::CreateWith("ac", a);
        TransformPtr b = Transform::CreateWith("b", root);
        TransformPtr ba = Transform::CreateWith("ba", b);
        TransformPtr bb = Transform::CreateWith("bb", b);
        TransformPtr bc = Transform::CreateWith("bc", b);
        TransformPtr c = Transform::CreateWith("c", root);
        TransformPtr ca = Transform::CreateWith("ca", c);
        TransformPtr caa = Transform::CreateWith("caa", ca);
        TransformPtr caaa = Transform::CreateWith("caaa", caa);
        TransformPtr caaaa = Transform::CreateWith("caaaa", caaa);
        TransformPtr d = Transform::CreateWith("d", root);
        std::vector<std::string> results;
        root->foreachRecursive([&results](int recurseDepth, const std::string& obj)
        {
            results.push_back(std::to_string(recurseDepth) + " " + obj);
        });
        int k=0;
        REQUIRE(results.at(k++) == "0 root");
        REQUIRE(results.at(k++) == "1 a");
        REQUIRE(results.at(k++) == "2 aa");
        REQUIRE(results.at(k++) == "2 ab");
        REQUIRE(results.at(k++) == "3 aba");
        REQUIRE(results.at(k++) == "3 abb");
        REQUIRE(results.at(k++) == "3 abc");
        REQUIRE(results.at(k++) == "4 abca");
        REQUIRE(results.at(k++) == "3 abd");
        REQUIRE(results.at(k++) == "2 ac");
        REQUIRE(results.at(k++) == "1 b");
        REQUIRE(results.at(k++) == "2 ba");
        REQUIRE(results.at(k++) == "2 bb");
        REQUIRE(results.at(k++) == "2 bc");
        REQUIRE(results.at(k++) == "1 c");
        REQUIRE(results.at(k++) == "2 ca");
        REQUIRE(results.at(k++) == "3 caa");
        REQUIRE(results.at(k++) == "4 caaa");
        REQUIRE(results.at(k++) == "5 caaaa");
        REQUIRE(results.at(k++) == "1 d");
    }
}
