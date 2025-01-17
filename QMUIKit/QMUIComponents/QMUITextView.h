/**
 * Tencent is pleased to support the open source community by making QMUI_iOS available.
 * Copyright (C) 2016-2021 THL A29 Limited, a Tencent company. All rights reserved.
 * Licensed under the MIT License (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
 */

//
//  QMUITextView.h
//  qmui
//
//  Created by QMUI Team on 14-8-5.
//

#import <UIKit/UIKit.h>

@class QMUITextView;

@protocol QMUITextViewDelegate <UITextViewDelegate>

@optional
/**
 *  输入框高度发生变化时的回调，当实现了这个方法后，文字输入过程中就会不断去计算输入框新内容的高度，并通过这个方法通知到 delegate
 *  @note 只有当内容高度与当前输入框的高度不一致时才会调用到这里，所以无需在内部做高度是否变化的判断。
 */
- (void)textView:(QMUITextView *)textView newHeightAfterTextChanged:(CGFloat)height;

/**
 *  用户点击键盘的 return 按钮时的回调（return 按钮本质上是输入换行符“\n”）
 *  @return 返回 YES 表示程序认为当前的点击是为了进行类似“发送”之类的操作，所以最终“\n”并不会被输入到文本框里。返回 NO 表示程序认为当前的点击只是普通的输入，所以会继续询问 textView:shouldChangeTextInRange:replacementText: 方法，根据该方法的返回结果来决定是否要输入这个“\n”。
 *  @see maximumTextLength
 */
- (BOOL)textViewShouldReturn:(QMUITextView *)textView;

/**
 由于 maximumTextLength 的实现方式导致业务无法再重写自己的 shouldChangeCharacters，否则会丢失 maximumTextLength 的功能。所以这里提供一个额外的 delegate，在 QMUI 内部逻辑返回 YES 的时候会再询问一次这个 delegate，从而给业务提供一个机会去限制自己的输入内容。如果 QMUI 内部逻辑本身就返回 NO（例如超过了 maximumTextLength 的长度），则不会触发这个方法。
 当输入被这个方法拦截时，由于拦截逻辑是业务自己写的，业务能轻松获取到这个拦截的时机，所以此时不会调用 textView:didPreventTextChangeInRange:replacementText:。如果有类似 tips 之类的操作，可以直接在 return NO 之前处理。
 */
- (BOOL)textView:(QMUITextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)text originalValue:(BOOL)originalValue;

/**
 *  配合 `maximumTextLength` 属性使用，在输入文字超过限制时被调用（此时文字已被自动裁剪到符合最大长度要求）。
 *
 *  @param textView 触发的 textView
 *  @param range 要变化的文字的位置，length > 0 表示文字被自动裁剪前，输入框已有一段文字被选中。
 *  @param replacementText 要变化的文字
 */
- (void)textView:(QMUITextView *)textView didPreventTextChangeInRange:(NSRange)range replacementText:(NSString *)replacementText;

@end

/**
 *  自定义 UITextView，提供的特性如下：
 *
 *  1. 支持 placeholder 并支持更改 placeholderColor；若使用了富文本文字，则 placeholder 的样式也会跟随文字的样式（除了 placeholder 颜色）
 *  2. 支持在文字发生变化时计算内容高度并通知 delegate。
 *  3. 支持限制输入框最大高度，一般配合第 2 点使用。
 *  4. 支持限制输入的文本的最大长度，默认不限制。
 *  5. 修正系统 UITextView 在输入时自然换行的时候，contentOffset 的滚动位置没有考虑 textContainerInset.bottom
 */
@interface QMUITextView : UITextView

@property(nonatomic, weak) id<QMUITextViewDelegate> delegate;

/**
 *  当通过 `setText:`、`setAttributedText:`等方式修改文字时，是否应该自动触发 `UITextViewDelegate` 里的 `textView:shouldChangeTextInRange:replacementText:`、 `textViewDidChange:` 方法
 *
 *  默认为YES（注意系统的 UITextView 对这种行为默认是 NO）
 */
@property(nonatomic, assign)  BOOL shouldResponseToProgrammaticallyTextChanges;

/**
 *  显示允许输入的最大文字长度，默认为 NSUIntegerMax，也即不限制长度。
 */
@property(nonatomic, assign)  NSUInteger maximumTextLength;

/**
 *  在使用 maximumTextLength 功能的时候，是否应该把文字长度按照 [NSString (QMUI) qmui_lengthWhenCountingNonASCIICharacterAsTwo] 的方法来计算。
 *  默认为 NO。
 */
@property(nonatomic, assign)  BOOL shouldCountingNonASCIICharacterAsTwo;

/**
 *   placeholder 的文字
 */
@property(nonatomic, copy)  NSString *placeholder;

/**
 *  placeholder 文字的颜色
 */
@property(nonatomic, strong)  UIColor *placeholderColor;

/**
 *  placeholder 在默认位置上的偏移（默认位置会自动根据 textContainerInset、contentInset 来调整）
 */
@property(nonatomic, assign) UIEdgeInsets placeholderMargins;

/**
 *  最大高度，当设置了这个属性后，超过这个高度值的 frame 是不生效的。默认为 CGFLOAT_MAX，也即无限制。
 */
@property(nonatomic, assign) CGFloat maximumHeight;

/**
 *  控制输入框是否要出现“粘贴”menu
 *  @param sender 触发这次询问事件的来源
 *  @param superReturnValue [super canPerformAction:withSender:] 的返回值，当你不需要控制这个 block 的返回值时，可以返回 superReturnValue
 *  @return 控制是否要出现“粘贴”menu，YES 表示出现，NO 表示不出现。当你想要返回系统默认的结果时，请返回参数 superReturnValue
 */
@property(nonatomic, copy) BOOL (^canPerformPasteActionBlock)(id sender, BOOL superReturnValue);

/**
 *  当输入框的“粘贴”事件被触发时，可通过这个 block 去接管事件的响应。
 *  @param sender “粘贴”事件触发的来源，例如可能是一个 UIMenuController
 *  @return 返回值用于控制是否要调用系统默认的 paste: 实现，YES 表示执行完 block 后继续调用系统默认实现，NO 表示执行完 block 后就结束了，不调用 super。
 */
@property(nonatomic, copy) BOOL (^pasteBlock)(id sender);

@end
