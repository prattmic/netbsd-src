/*	$NetBSD: octeon_ipdvar.h,v 1.5 2020/06/23 05:15:33 simonb Exp $	*/

/*
 * Copyright (c) 2007 Internet Initiative Japan, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _OCTEON_IPDVAR_H_
#define _OCTEON_IPDVAR_H_

/* XXX */
struct octipd_softc {
	int			sc_port;
	bus_space_tag_t		sc_regt;
	bus_space_handle_t	sc_regh;
	size_t			sc_first_mbuff_skip;
	size_t			sc_not_first_mbuff_skip;
};

/* XXX */
struct octipd_attach_args {
	int			aa_port;
	bus_space_tag_t		aa_regt;
	size_t			aa_first_mbuff_skip;
	size_t			aa_not_first_mbuff_skip;
};

void	octipd_init(struct octipd_attach_args *, struct octipd_softc **);
int	octipd_enable(struct octipd_softc *);
int	octipd_config(struct octipd_softc *);
void	octipd_sub_port_fcs(struct octipd_softc *, int);
void	octipd_offload(uint64_t, void *, int *);

#endif /* _OCTEON_IPDVAR_H_ */
