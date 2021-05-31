/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/speedreader/speedreader_bubble_single_page.h"

#include <memory>
#include <string>
#include <utility>

#include "base/bind.h"
#include "base/notreached.h"
#include "brave/browser/ui/speedreader/speedreader_bubble_controller.h"
#include "brave/browser/ui/views/speedreader/speedreader_bubble_util.h"
#include "brave/common/url_constants.h"
#include "brave/grit/brave_generated_resources.h"
#include "chrome/browser/ui/views/location_bar/location_bar_bubble_delegate_view.h"
#include "chrome/grit/generated_resources.h"
#include "components/strings/grit/components_strings.h"
#include "content/public/browser/page_navigator.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_ui_controller.h"
#include "content/public/common/referrer.h"
#include "include/core/SkColor.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/window_open_disposition.h"
#include "ui/events/event.h"
#include "ui/views/controls/button/md_text_button.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/styled_label.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/metadata/metadata_impl_macros.h"
#include "ui/views/view.h"

namespace {

constexpr int kBubbleWidth = 264;  // width is 264 pixels

}  // anonymous namespace

// Material Design button, overriding the font list in the LabelButton.
class ReaderButton : public views::MdTextButton {
 public:
  explicit ReaderButton(PressedCallback callback = PressedCallback(),
                        const std::u16string& text = std::u16string(),
                        int button_context = views::style::CONTEXT_BUTTON_MD)
      : views::MdTextButton(callback, text, button_context) {
    label()->SetFontList(speedreader::GetFont(13, gfx::Font::Weight::SEMIBOLD));
  }

  void SetEnabledColor(SkColor color) { label()->SetEnabledColor(color); }
  ~ReaderButton() override = default;
};

SpeedreaderBubbleSinglePage::SpeedreaderBubbleSinglePage(
    views::View* anchor_view,
    content::WebContents* web_contents,
    SpeedreaderBubbleController* controller)
    : LocationBarBubbleDelegateView(anchor_view, nullptr),
      web_contents_(web_contents),
      controller_(controller) {
  SetButtons(ui::DialogButton::DIALOG_BUTTON_NONE);
}

void SpeedreaderBubbleSinglePage::Show() {
  ShowForReason(USER_GESTURE);
}

void SpeedreaderBubbleSinglePage::Hide() {
  if (controller_) {
    controller_->OnBubbleClosed();
    controller_ = nullptr;
  }
  CloseBubble();
}

gfx::Size SpeedreaderBubbleSinglePage::CalculatePreferredSize() const {
  return gfx::Size(
      kBubbleWidth,
      LocationBarBubbleDelegateView::CalculatePreferredSize().height());
}

bool SpeedreaderBubbleSinglePage::ShouldShowCloseButton() const {
  return true;
}

void SpeedreaderBubbleSinglePage::WindowClosing() {
  if (controller_) {
    controller_->OnBubbleClosed();
    controller_ = nullptr;
  }
}

void SpeedreaderBubbleSinglePage::Init() {
  SetLayoutManager(std::make_unique<views::BoxLayout>(
      views::BoxLayout::Orientation::kVertical, gfx::Insets(),
      speedreader::kBoxLayoutChildSpacing));

  // Heading
  auto heading_label = std::make_unique<views::Label>(
      l10n_util::GetStringUTF16(IDS_SPEEDREADER_ASK_ENABLE));
  heading_label->SetMultiLine(true);
  heading_label->SetFontList(
      speedreader::GetFont(16, gfx::Font::Weight::SEMIBOLD));
  heading_label->SetHorizontalAlignment(gfx::ALIGN_LEFT);
  heading_label_ = AddChildView(std::move(heading_label));

  // Explanation of Speedreader features
  auto global_toggle_label = speedreader::BuildLabelWithEndingLink(
      l10n_util::GetStringUTF16(IDS_SPEEDREADER_EXPLANATION),
      l10n_util::GetStringUTF16(IDS_LEARN_MORE),
      base::BindRepeating(&SpeedreaderBubbleSinglePage::OnLinkClicked,
                          base::Unretained(this)));
  global_toggle_label->SetHorizontalAlignment(gfx::ALIGN_LEFT);
  global_toggle_label->SetLineHeight(speedreader::kLineHeight);
  global_toggle_label_ = AddChildView(std::move(global_toggle_label));

  // Enable Speedreader button
  auto enable_speedreader_button = std::make_unique<ReaderButton>(
      base::BindRepeating(&SpeedreaderBubbleSinglePage::OnButtonPressed,
                          base::Unretained(this)),
      l10n_util::GetStringUTF16(IDS_SPEEDREADER_ENABLE_BUTTON));
  enable_speedreader_button_ =
      AddChildView(std::move(enable_speedreader_button));
}

void SpeedreaderBubbleSinglePage::OnButtonPressed(const ui::Event& event) {
  // FIXME: Tie up this logic to the speedreader service. Enable Speedreader
  // globally.
  NOTIMPLEMENTED();
}

void SpeedreaderBubbleSinglePage::OnLinkClicked(const ui::Event& event) {
  web_contents_->OpenURL(content::OpenURLParams(
      GURL(kSpeedreaderLearnMoreUrl), content::Referrer(),
      WindowOpenDisposition::NEW_FOREGROUND_TAB, ui::PAGE_TRANSITION_LINK,
      false));
}

BEGIN_METADATA(SpeedreaderBubbleSinglePage, LocationBarBubbleDelegateView)
END_METADATA
