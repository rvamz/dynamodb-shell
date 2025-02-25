// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
//
/*
 * select.hpp
 *
 * the class definition for CSelect
 *
 * Amrith - 2022-10-07
 */

#ifndef __SELECT_COMMAND_HPP_DEFINED__
#define __SELECT_COMMAND_HPP_DEFINED__

#include "aws-includes.hpp"
#include "parser-defs.hpp"
#include "logging.hpp"
#include "command.hpp"
#include "symbol_table.hpp"
#include "select_helper.hpp"
#include "ratelimit.hpp"

namespace ddbsh
{

    class CSelectCommand : public CCommand
    {
    public:
        bool setup (CRateLimit * ratelimit, bool consistent, Aws::Vector<Aws::String> * projection,
                    Aws::Vector<Aws::String> * table, CWhere * where, Aws::DynamoDB::Model::ReturnConsumedCapacity returns) {
            m_ratelimit = ratelimit;
            m_returns = returns;
            if (m_helper.setup(consistent, projection, table, where, returns, ratelimit ? true : false))
                return false;

            m_where = where;
            return true;
        };

        CSelectCommand() {
            m_exists = false;
            m_ratelimit = NULL;
            m_where = NULL;
        };

        ~CSelectCommand() {
            logdebug("[%s, %d] In destructor.\n", __FILENAME__, __LINE__);
            delete m_ratelimit;
            delete m_where;
        };

        virtual int run();

        virtual Aws::DynamoDB::Model::TransactGetItem * txget();
        Aws::Vector<Aws::DynamoDB::Model::TransactWriteItem> * txwrite();

        virtual void exists() final {
            m_exists = true;
        };

    private:
        CSelectHelper m_helper;
        CRateLimit * m_ratelimit;
        Aws::DynamoDB::Model::ReturnConsumedCapacity m_returns;
        bool m_exists;

        CWhere * m_where; // this is the holder for m_where which came
                          // from parser, solely for the destructor.
        int do_scan();
        int do_query();
        int do_getitem();

        void display_consumed_capacity(const Aws::DynamoDB::Model::ConsumedCapacity cc);
    };

}
#endif
