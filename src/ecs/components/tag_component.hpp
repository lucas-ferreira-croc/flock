#ifndef TAG_COMPONENT_HPP
#define TAG_COMPONENT_HPP

enum class TagType
{
    RegularEntity = 0,
    EditModeEntity = 1,
};

struct TagComponent
{
    TagComponent(TagType tagType = TagType::RegularEntity)
    {
        mTagType = tagType;
    }

    TagType mTagType;
    bool show = false;
};

#endif