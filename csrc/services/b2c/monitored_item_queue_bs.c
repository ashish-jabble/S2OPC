/*
 *  Copyright (C) 2018 Systerel and others.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY{} without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "monitored_item_queue_bs.h"

#include <assert.h>

/*--------------
   SEES Clause
  --------------*/
#include "constants.h"

#include "monitored_item_pointer_impl.h"

/*------------------------
   INITIALISATION Clause
  ------------------------*/
void monitored_item_queue_bs__INITIALISATION(void) {}

/*--------------------
   OPERATIONS Clause
  --------------------*/
void monitored_item_queue_bs__allocate_new_monitored_item_queue(
    t_bool* const monitored_item_queue_bs__bres,
    constants__t_monitoredItemQueue_i* const monitored_item_queue_bs__queue)
{
    *monitored_item_queue_bs__queue = SOPC_SLinkedList_Create(0);
    if (*monitored_item_queue_bs__queue == NULL)
    {
        *monitored_item_queue_bs__bres = false;
    }
    else
    {
        *monitored_item_queue_bs__bres = true;
    }
}

void monitored_item_queue_bs__clear_and_deallocate_monitored_item_queue(
    const constants__t_monitoredItemQueue_i monitored_item_queue_bs__p_queue)
{
    SOPC_SLinkedList_Delete(monitored_item_queue_bs__p_queue);
}

void monitored_item_queue_bs__add_monitored_item_to_queue(
    const constants__t_monitoredItemQueue_i monitored_item_queue_bs__p_queue,
    const constants__t_monitoredItemPointer_i monitored_item_queue_bs__p_monitoredItem,
    t_bool* const monitored_item_queue_bs__bres)
{
    SOPC_InternalMontitoredItem* mi = (SOPC_InternalMontitoredItem*) monitored_item_queue_bs__p_monitoredItem;
    SOPC_SLinkedList_Append(monitored_item_queue_bs__p_queue, mi->monitoredItemId,
                            monitored_item_queue_bs__p_monitoredItem);
}

void monitored_item_queue_bs__continue_iter_monitored_item(
    const constants__t_monitoredItemQueueIterator_i monitored_item_queue_bs__p_iterator,
    const constants__t_monitoredItemQueue_i monitored_item_queue_bs__p_queue,
    t_bool* const monitored_item_queue_bs__continue,
    constants__t_monitoredItemPointer_i* const monitored_item_queue_bs__p_monitoredItem)
{
    (void) monitored_item_queue_bs__p_queue;
    SOPC_GCC_DIAGNOSTIC_IGNORE_CAST_CONST
    SOPC_SLinkedListIterator it = (SOPC_SLinkedListIterator) monitored_item_queue_bs__p_iterator;
    SOPC_GCC_DIAGNOSTIC_RESTORE
    *monitored_item_queue_bs__continue = false;
    *monitored_item_queue_bs__p_monitoredItem = SOPC_SLinkedList_Next(&it);
    if (NULL != *monitored_item_queue_bs__p_monitoredItem)
    {
        *monitored_item_queue_bs__continue = true;
    }
}

void monitored_item_queue_bs__init_iter_monitored_item(
    const constants__t_monitoredItemQueue_i monitored_item_queue_bs__p_queue,
    t_bool* const monitored_item_queue_bs__continue,
    constants__t_monitoredItemQueueIterator_i* const monitored_item_queue_bs__iterator)
{
    *monitored_item_queue_bs__continue = false;
    if (SOPC_SLinkedList_GetLength(monitored_item_queue_bs__p_queue) > 0)
    {
        *monitored_item_queue_bs__iterator = SOPC_SLinkedList_GetIterator(monitored_item_queue_bs__p_queue);
        if (*monitored_item_queue_bs__iterator != NULL)
        {
            *monitored_item_queue_bs__continue = true;
        }
    }
}

void monitored_item_queue_bs__remove_monitored_item(
    const constants__t_monitoredItemQueue_i monitored_item_queue_bs__p_queue,
    const constants__t_monitoredItemPointer_i monitored_item_queue_bs__p_monitoredItem,
    t_bool* const monitored_item_queue_bs__bres)
{
    SOPC_InternalMontitoredItem* mi = (SOPC_InternalMontitoredItem*) monitored_item_queue_bs__p_monitoredItem;

    constants__t_monitoredItemPointer_i* res =
        SOPC_SLinkedList_RemoveFromId(monitored_item_queue_bs__p_queue, mi->monitoredItemId);
    *monitored_item_queue_bs__bres = res != NULL;
}
