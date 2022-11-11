#include "car-event-log-table.h"

#include <memory>
#include <string>
#include <map>
#include <sstream>

ftxui::Component CarEventLogTable(std::vector<CarEventTableEntrySP> &overtakeLog, std::function<void(int frameNum, int carIdx)> on_click)
{
    using namespace ftxui;

    class Impl : public ComponentBase
    {
    public:
        Impl(std::vector<CarEventTableEntrySP> *overtakeLog,
             std::function<void(int frameNum, int carIdx)> on_click)
            : overtakeLog_(overtakeLog),
              on_click_(std::move(on_click)) {}

        // Component implementation:
        Element Render() override
        {
            logRow2CarIdx_.clear();

            ftxui::Elements nameElements({
                text("Name"),
            });
            ftxui::Elements noteElements({
                text("Notes"),
            });
            ftxui::Elements spacer1({text("  ")});
            ftxui::Elements spacer2({text("  ")});
            ftxui::Elements franeNumElements({text("Frame Num")});

            int rowIdx = 0;

            for (auto it = overtakeLog_->rbegin(); it != overtakeLog_->rend(); ++it)
            {
                auto ev = *it;
                auto color1 = Color::Green;

                if (ev->seen)
                {
                    color1 = Color::GrayDark;
                }

                if (rowIdx == hoverRow_)
                {
                    color1 = Color::GrayLight;
                }

                logRow2CarIdx_[rowIdx] = ev->carIdx;
                logRow2CFrameNum_[rowIdx] = ev->frameNumber;
                ++rowIdx;

                nameElements.push_back(color(color1, text(ev->driverName)));
                noteElements.push_back(color(color1, text(ev->eventNote)));
                // spacer1.push_back(text("  "));

                std::stringstream sout;
                sout << ev->frameNumber;
                franeNumElements.push_back(color(color1, text(sout.str())));
            }

            ftxui::Elements e({
                hbox({
                    vbox(franeNumElements),
                    vbox(spacer1),
                    vbox(nameElements),
                    vbox(spacer2),
                    vbox(noteElements) | flex,

                }) | vscroll_indicator |
                    border,
            });
            auto r = vbox(e);

            return r | reflect(box_);
        }

        Decorator AnimatedColorStyle()
        {
            Decorator style = nothing;
            if (option_->animated_colors.background.enabled)
            {
                style = style | bgcolor(Color::Interpolate(
                                    animation_foreground_, //
                                    option_->animated_colors.background.inactive,
                                    option_->animated_colors.background.active));
            }
            if (option_->animated_colors.foreground.enabled)
            {
                style = style | color(Color::Interpolate(
                                    animation_foreground_, //
                                    option_->animated_colors.foreground.inactive,
                                    option_->animated_colors.foreground.active));
            }
            return style;
        }

        void SetAnimationTarget(float target)
        {
            if (option_->animated_colors.foreground.enabled)
            {
                animator_foreground_ =
                    animation::Animator(&animation_foreground_, target,
                                        option_->animated_colors.foreground.duration,
                                        option_->animated_colors.foreground.function);
            }
            if (option_->animated_colors.background.enabled)
            {
                animator_background_ =
                    animation::Animator(&animation_background_, target,
                                        option_->animated_colors.background.duration,
                                        option_->animated_colors.background.function);
            }
        }

        void OnAnimation(animation::Params &p) override
        {
            animator_background_.OnAnimation(p);
            animator_foreground_.OnAnimation(p);
        }

        void OnClick(int x, int y)
        {
            int row = y - box_.y_min - 2;
            if (logRow2CarIdx_.count(row) && logRow2CFrameNum_.count(row))
            {
                int carIdx = logRow2CarIdx_[row];
                int frameNum = logRow2CFrameNum_[row];
                on_click_(frameNum, carIdx);
            }

            // animation_background_ = 0.5F; // NOLINT
            // animation_foreground_ = 0.5F; // NOLINT
            // SetAnimationTarget(1.F);      // NOLINT
        }

        bool OnEvent(Event event) override
        {
            if (event.is_mouse())
            {
                return OnMouseEvent(event);
            }

            // if (event == Event::Return)
            // {
            //     OnClick();
            //     return true;
            // }
            return false;
        }

        bool OnMouseEvent(Event event)
        {
            mouse_hover_ =
                box_.Contain(event.mouse().x, event.mouse().y) && CaptureMouse(event);

            if (!mouse_hover_)
            {
                return false;
            }

            hoverRow_ = event.mouse().y - box_.y_min - 2;

            if (event.mouse().button == Mouse::Left &&
                event.mouse().motion == Mouse::Pressed)
            {
                TakeFocus();
                OnClick(event.mouse().x, event.mouse().y);
                return true;
            }

            return false;
        }

        bool Focusable() const final { return true; }

    private:
        std::vector<CarEventTableEntrySP> *overtakeLog_;
        std::function<void(int frameNum, int carIdx)> on_click_;
        std::map<int, int> logRow2CarIdx_;
        std::map<int, int> logRow2CFrameNum_;
        bool mouse_hover_ = false;
        Box box_;
        int hoverRow_ = -1;
        Ref<ButtonOption> option_;
        float animation_background_ = 0;
        float animation_foreground_ = 0;
        animation::Animator animator_background_ =
            animation::Animator(&animation_background_);
        animation::Animator animator_foreground_ =
            animation::Animator(&animation_foreground_);
    };

    return Make<Impl>(&overtakeLog, std::move(on_click));
}