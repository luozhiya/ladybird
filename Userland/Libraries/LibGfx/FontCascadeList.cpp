/*
 * Copyright (c) 2023, Aliaksandr Kalenik <kalenik.aliaksandr@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibGfx/FontCascadeList.h>

namespace Gfx {

void FontCascadeList::add(NonnullRefPtr<Font> font)
{
    m_fonts.append({ move(font), {} });
}

void FontCascadeList::add(NonnullRefPtr<Font> font, Vector<UnicodeRange> unicode_ranges)
{
    m_fonts.append({ move(font), move(unicode_ranges) });
}

void FontCascadeList::extend(FontCascadeList const& other)
{
    for (auto const& font : other.m_fonts) {
        m_fonts.append({ font.font, font.unicode_ranges });
    }
}

// https://drafts.csswg.org/css-fonts/#descdef-font-face-unicode-range
// When the font is used, the effective character map is the **intersection** of
// the codepoints defined by unicode-range with the font’s character map.
Optional<Gfx::Font const&> FontCascadeList::font_for_code_point(u32 code_point) const
{
    for (auto const& entry : m_fonts) {
        if (!entry.font->contains_glyph(code_point))
            continue;
        if (!entry.unicode_ranges.has_value())
            return { entry.font };
        for (auto const& range : *entry.unicode_ranges) {
            if (range.contains(code_point))
                return { entry.font };
        }
    }
    return {};
}

void FontCascadeList::for_each_font(Function<bool(Font const&)> callback) const
{
    for (auto const& entry : m_fonts) {
        if (callback(entry.font))
            break;
    }
}

bool FontCascadeList::equals(FontCascadeList const& other) const
{
    if (m_fonts.size() != other.m_fonts.size())
        return false;
    for (size_t i = 0; i < m_fonts.size(); ++i) {
        if (m_fonts[i].font != other.m_fonts[i].font)
            return false;
    }
    return true;
}

}
