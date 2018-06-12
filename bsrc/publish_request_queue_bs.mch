/*
 *  Copyright (C) 2018 Systerel and others.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

MACHINE
    publish_request_queue_bs

SEES
    constants,
    message_out_bs,
    request_handle_bs
    
ABSTRACT_VARIABLES

    s_publishReqQueue,
    a_publish_responses,
    a_publish_expiration_time,
    s_expired_time_ref,
    validPublishReqQueued
    
INVARIANT
    s_publishReqQueue <: t_publishReqQueue &
    a_publish_responses : t_msg +-> t_publishReqQueue &
    a_publish_expiration_time : t_msg +-> t_timeref &
    dom(a_publish_expiration_time) = dom(a_publish_responses) &
    s_expired_time_ref <: t_timeref & /* Represents the set of expired time references (for model only) */
    validPublishReqQueued : BOOL
    
INITIALISATION
    s_publishReqQueue :(s_publishReqQueue = {}) ||
    a_publish_responses :(a_publish_responses = {}) ||
    a_publish_expiration_time :(a_publish_expiration_time = {}) ||
    s_expired_time_ref :(s_expired_time_ref <: t_timeref) ||
    validPublishReqQueued := FALSE

OPERATIONS
    
    bres, queue <-- allocate_new_publish_queue =
    BEGIN
        IF s_publishReqQueue = t_publishReqQueue
        THEN
            bres := FALSE ||
            queue := c_publishReqQueue_indet
        ELSE
            ANY l_queue WHERE
                l_queue : t_publishReqQueue_i &
                l_queue : t_publishReqQueue &
                l_queue /: s_publishReqQueue
            THEN
                s_publishReqQueue := s_publishReqQueue \/ {l_queue} ||
                queue := l_queue ||
                bres := TRUE
            END
        END
    END
    ;
    
    clear_publish_queue (p_queue) =
    PRE
        p_queue : t_publishReqQueue_i &
        p_queue : s_publishReqQueue
    THEN
        a_publish_responses :(a_publish_responses : t_msg +-> t_publishReqQueue &
                              a_publish_responses |> {p_queue} = {} &
                              a_publish_responses |>> {p_queue} = a_publish_responses$0 |>> {p_queue})
    END
    ;
    
    clear_and_deallocate_publish_queue (p_queue) =
    PRE
        p_queue : t_publishReqQueue_i &
        p_queue : s_publishReqQueue
    THEN
        a_publish_responses :(a_publish_responses : t_msg +-> t_publishReqQueue &
                              a_publish_responses |> {p_queue} = {} &
                              a_publish_responses |>> {p_queue} = a_publish_responses$0 |>> {p_queue}) ||
        s_publishReqQueue := s_publishReqQueue - {p_queue}
    END
    ;
    
    bres <-- append_publish_request_to_queue (p_queue, p_session, p_req_exp_time, p_req_handle, p_req_ctx, p_resp_msg) =
    PRE
        p_queue : t_publishReqQueue_i &
        p_queue : s_publishReqQueue &
        p_session : t_session_i &
        p_session : t_session &
        p_req_exp_time : t_timeref_i &
        p_req_exp_time : t_timeref &
        p_req_handle : t_server_request_handle_i &
        p_req_ctx : t_request_context_i &
        p_req_ctx : t_request_context &
        p_resp_msg : t_msg_i &
        p_resp_msg : t_msg &
        p_resp_msg = c_msg_out &
        a_msg_out_type = e_msg_subscription_publish_resp
    THEN
        CHOICE
            bres := FALSE
        OR 
            bres := TRUE ||
            a_publish_responses := a_publish_responses \/ {p_resp_msg |-> p_queue} ||
            a_publish_expiration_time := a_publish_expiration_time \/ {p_resp_msg |-> p_req_exp_time}
        END
    END
    ;
    
    continue <-- init_iter_publish_request (p_queue) =
    PRE
        p_queue : t_publishReqQueue_i &
        p_queue : s_publishReqQueue
    THEN
        continue := bool(a_publish_responses~[{p_queue}] /= {})
    END
    ;

    p_continue, p_session, p_req_exp_time, p_req_handle, p_req_ctx, p_resp_msg
      <-- continue_pop_head_iter_publish_request (p_queue) =
    PRE
        p_queue : t_publishReqQueue_i &
        p_queue : s_publishReqQueue &
        a_publish_responses~[{p_queue}] /= {}
    THEN
        ANY l_resp_msg WHERE
            l_resp_msg : t_msg_i &
            l_resp_msg : t_msg &
            l_resp_msg |-> p_queue : a_publish_responses
        THEN
            p_continue := bool(a_publish_responses~[{p_queue}] /= {l_resp_msg}) ||
            a_publish_responses := a_publish_responses - {l_resp_msg |-> p_queue} ||
            p_resp_msg := l_resp_msg ||
            a_publish_expiration_time, p_session, p_req_exp_time, p_req_handle, p_req_ctx
            :(  p_session : t_session_i &
                p_session : t_session &
                p_req_exp_time : t_timeref_i &
                p_req_exp_time : t_timeref &
                p_req_handle : t_server_request_handle_i &
                p_req_ctx : t_request_context_i &
                p_req_ctx : t_request_context &
                a_publish_expiration_time = a_publish_expiration_time$0 - {l_resp_msg |-> p_req_exp_time}
            )
        END
    END
    ;
    
    bres <-- is_request_expired (p_req_exp_time) =
    PRE
        p_req_exp_time : t_timeref_i &
        p_req_exp_time : t_timeref
    THEN
        s_expired_time_ref,
        bres
        :(s_expired_time_ref <: t_timeref & bres = bool(p_req_exp_time /: s_expired_time_ref))
        /* Update s_expired_time_ref and evaluation of p_req_exp_time expiration */
    END
    ;
    
    /* Used to re-enqueue the first non expired publish request found in queue */
    bres <-- prepend_publish_request_to_queue (p_queue, p_session, p_req_exp_time, p_req_handle, p_req_ctx, p_resp_msg) =
    PRE
        p_queue : t_publishReqQueue_i &
        p_queue : s_publishReqQueue &
        p_session : t_session_i &
        p_session : t_session &
        p_req_exp_time : t_timeref_i &
        p_req_exp_time : t_timeref &
        p_req_exp_time /: s_expired_time_ref & /* Validated as non expired time reference only */
        p_req_handle : t_server_request_handle_i &
        p_req_ctx : t_request_context_i &
        p_req_ctx : t_request_context &
        p_resp_msg : t_msg_i &
        p_resp_msg : t_msg &
        p_resp_msg = c_msg_out &
        a_msg_out_type = e_msg_subscription_publish_resp
    THEN
        CHOICE
            bres := FALSE
        OR 
            bres := TRUE ||
            a_publish_responses := a_publish_responses \/ {p_resp_msg |-> p_queue} ||
            a_publish_expiration_time := a_publish_expiration_time \/ {p_resp_msg |-> p_req_exp_time} ||
            validPublishReqQueued := TRUE
        END
    END
    ;
    
    /* Used to dequeue a non expired publish request:  */
    p_session, p_req_exp_time, p_req_handle, p_req_ctx, p_resp_msg
      <-- pop_valid_publish_request_queue (p_queue) =
    PRE
        p_queue : t_publishReqQueue_i &
        p_queue : s_publishReqQueue &
        a_publish_responses~[{p_queue}] /= {} &
        validPublishReqQueued = TRUE
    THEN
        ANY l_resp_msg WHERE
            l_resp_msg : t_msg_i &
            l_resp_msg : t_msg &
            l_resp_msg |-> p_queue : a_publish_responses
        THEN
            a_publish_responses := a_publish_responses - {l_resp_msg |-> p_queue} ||
            validPublishReqQueued := FALSE ||
            p_resp_msg := l_resp_msg ||
            a_publish_expiration_time, p_session, p_req_exp_time, p_req_handle, p_req_ctx
            :(
                p_session : t_session_i &
                p_session : t_session &
                p_req_exp_time : t_timeref_i &
                p_req_exp_time : t_timeref &
                p_req_exp_time /: s_expired_time_ref & /* Validated as non expired time reference only */
                p_req_handle : t_server_request_handle_i &
                p_req_ctx : t_request_context_i &
                p_req_ctx : t_request_context &
                a_publish_expiration_time = a_publish_expiration_time$0 - {l_resp_msg |-> p_req_exp_time}
            )
        END
    END    
    
END