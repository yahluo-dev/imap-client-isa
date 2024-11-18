/**
 * @file imf_message.cpp
 * @brief Tests for IMFMessage class
 * @author Iaroslav Vasilevskii (xvasil10)
 */
#include <gtest/gtest.h>
#include "../../imf_message.hpp"
#include <stdexcept>

TEST(IMFMessageTests, GetDatetimeFormatted)
{
  IMFMessage imf_message("Return-Path: <jerry@josh-20l6s67300.localdomain>\r\n"
                          "X-Original-To: josh@josh-20l6s67300.localdomain\r\n"
                          "Delivered-To: josh@josh-20l6s67300.localdomain\r\n"
                          "Received: by josh-20l6s67300.localdomain (Postfix, from userid 30033)\r\n"
                          "        id 32E1618D1; Thu, 17 Oct 2024 13:01:30 +0200 (CEST)\r\n"
                          "Date: Thu, 17 Oct 2024 13:01:30 +0200\r\n"
                          "To: josh@josh-20l6s67300.localdomain\r\n"
                          "Subject: Test Email\r\n"
                          "User-Agent: mail v14.9.24\r\n"
                          "Message-Id: <20241017110130.32E1618D1@josh-20l6s67300.localdomain>\r\n"
                          "From: jerry@josh-20l6s67300.localdomain\r\n"
                          "\r\n");
  std::string result;
  ASSERT_NO_THROW(result = imf_message.get_datetime_formatted());
  ASSERT_EQ(result, "_Thu__17_Oct_2024_13_01_30_+0200");
}

TEST(IMFMessageTests, GetDatetimeFormattedMultipleDates)
{
  IMFMessage imf_message("Return-Path: <jerry@josh-20l6s67300.localdomain>\r\n"
                          "X-Original-To: josh@josh-20l6s67300.localdomain\r\n"
                          "Delivered-To: josh@josh-20l6s67300.localdomain\r\n"
                          "Received: by josh-20l6s67300.localdomain (Postfix, from userid 30033)\r\n"
                          "        id 32E1618D1; Thu, 17 Oct 2024 13:01:30 +0200 (CEST)\r\n"
                          "Date: Thu, 17 Oct 2024 13:01:30 +0200\r\n"
                          "Date: Thu, 18 Oct 2024 13:01:30 +0200\r\n"
                          "To: josh@josh-20l6s67300.localdomain\r\n"
                          "Subject: Test Email\r\n"
                          "User-Agent: mail v14.9.24\r\n"
                          "Message-Id: <20241017110130.32E1618D1@josh-20l6s67300.localdomain>\r\n"
                          "From: jerry@josh-20l6s67300.localdomain\r\n"
                          "\r\n");
  std::string result;
  ASSERT_NO_THROW(result = imf_message.get_datetime_formatted());
  ASSERT_EQ(result, "_Thu__17_Oct_2024_13_01_30_+0200");
}

TEST(IMFMessageTests, GetDatetimeFormattedNoDate)
{
  IMFMessage imf_message("Return-Path: <jerry@josh-20l6s67300.localdomain>\r\n"
                          "X-Original-To: josh@josh-20l6s67300.localdomain\r\n"
                          "Delivered-To: josh@josh-20l6s67300.localdomain\r\n"
                          "Received: by josh-20l6s67300.localdomain (Postfix, from userid 30033)\r\n"
                          "        id 32E1618D1; Thu, 17 Oct 2024 13:01:30 +0200 (CEST)\r\n"
                          "To: josh@josh-20l6s67300.localdomain\r\n"
                          "Subject: Test Email\r\n"
                          "User-Agent: mail v14.9.24\r\n"
                          "Message-Id: <20241017110130.32E1618D1@josh-20l6s67300.localdomain>\r\n"
                          "From: jerry@josh-20l6s67300.localdomain\r\n"
                          "\r\n");
  std::string result;
  ASSERT_THROW(result = imf_message.get_datetime_formatted(), std::runtime_error);
}
