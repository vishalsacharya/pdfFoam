/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 1991-2010 OpenCFD Ltd.
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "mcParticle.H"
#include "IOstreams.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::mcParticle::mcParticle
(
    const Cloud<mcParticle>& cloud,
    Istream& is,
    bool readFields
)
:
    Particle<mcParticle>(cloud, is, readFields)
{
    if (readFields)
    {
        if (is.format() == IOstream::ASCII)
        {
            m_ = readScalar(is);
            is >> Updf_;
            is >> UParticle_;
            is >> UFap_;
            is >> z_;
            is >> rho_;
            is >> dt_;
            is >> shift_;
            is >> ghost_;
        }
        else
        {
            is.read
            (
                reinterpret_cast<char*>(&m_),
                sizeof(m_) + sizeof(Updf_) +
                sizeof(UParticle_) + sizeof(UFap_) +
                sizeof(z_) + sizeof(rho_) + sizeof(dt_) +
                sizeof(shift_) + sizeof(ghost_)
            );
        }
    }

    // Check state of Istream
    is.check("mcParticle::mcParticle(Istream&)");
}


void Foam::mcParticle::readFields(Cloud<mcParticle>& c)
{
    if (!c.size())
    {
        return;
    }
    Particle <mcParticle> :: readFields(c);

    IOField<scalar> m(c.fieldIOobject("m", IOobject::MUST_READ));
    c.checkFieldIOobject(c, m);

    IOField<vector> Updf(c.fieldIOobject("Updf", IOobject::MUST_READ));
    c.checkFieldIOobject(c, Updf);

    IOField<vector> UParticle(c.fieldIOobject("UParticle", IOobject::MUST_READ));
    c.checkFieldIOobject(c, UParticle);

    IOField<vector> UFap(c.fieldIOobject("UFap", IOobject::MUST_READ));
    c.checkFieldIOobject(c, UFap);

    IOField<scalar> z(c.fieldIOobject("z", IOobject::MUST_READ));
    c.checkFieldIOobject(c, z);

    IOField<scalar> rho(c.fieldIOobject("rho", IOobject::MUST_READ));
    c.checkFieldIOobject(c, rho);

    label i = 0;
    forAllIter(Cloud<mcParticle>, c, iter)
    {
        mcParticle& p = iter();

        p.m_ = m[i];
        p.Updf_ = Updf[i];
        p.UParticle_ = UParticle[i];
        p.UFap_ = UFap[i];
        p.z_ = z[i];
        p.rho_ = rho[i];
        p.shift_ = vector::zero;
        p.ghost_ = 0;
        i++;
    }
}


void Foam::mcParticle::writeFields(const Cloud<mcParticle>& c)
{
    Particle<mcParticle>::writeFields(c);

    label np = c.size();

    IOField<scalar> m(c.fieldIOobject("m", IOobject::NO_READ), np);
    IOField<vector> Updf(c.fieldIOobject("Updf", IOobject::NO_READ), np);
    IOField<vector> UParticle(c.fieldIOobject("UParticle", IOobject::NO_READ), np);
    IOField<vector> UFap(c.fieldIOobject("UFap", IOobject::NO_READ), np);
    IOField<scalar> z(c.fieldIOobject("z", IOobject::NO_READ), np);
    IOField<scalar> rho(c.fieldIOobject("rho", IOobject::NO_READ), np);

    label i = 0;
    forAllConstIter(Cloud<mcParticle>, c, iter)
    {
        const mcParticle& p = iter();

        m[i] = p.m_;
        Updf[i] = p.Updf_;
        UParticle[i] = p.UParticle_;
        UFap[i] = p.UFap_;
        z[i] = p.z_;
        rho[i] = p.rho_;
        i++;
    }

    m.write();
    Updf.write();
    UParticle.write();
    UFap.write();
    z.write();
    rho.write();
}


// * * * * * * * * * * * * * * * IOstream Operators  * * * * * * * * * * * * //

Foam::Ostream& Foam::operator<<(Ostream& os, const mcParticle& p)
{
    if (os.format() == IOstream::ASCII)
    {
        os  << static_cast<const Particle<mcParticle>&>(p)
            << token::SPACE << p.m_
            << token::SPACE << p.Updf_
            << token::SPACE << p.UParticle_
            << token::SPACE << p.UFap_
            << token::SPACE << p.z_
            << token::SPACE << p.rho_
            << token::SPACE << p.dt_
            << token::SPACE << p.shift_
            << token::SPACE << p.ghost_;
    }
    else
    {
#if 0
        printf("\nDEBUG: Particle layout\n"
               "\t&m = %p\tsizeof(m) = %zu\n"
               "\t&Updf = %p\tsizeof(Updf) = %zu\n"
               "\t&UParticle = %p\tsizeof(UParticle) = %zu\n"
               "\t&UFap = %p\tsizeof(UFap) = %zu\n"
               "\t&z = %p\tsizeof(z) = %zu\n"
               "\t&rho = %p\tsizeof(rho) = %zu\n"
               "\t&dt = %p\tsizeof(dt) = %zu\n"
               "\t&shift = %p\tsizeof(shift) = %zu\n"
               "\t&ghost = %p\tsizeof(ghost) = %zu\n"
               "\n",
               &p.m_, sizeof(p.m_),
               &p.Updf_, sizeof(p.Updf_),
               &p.UParticle_, sizeof(p.UParticle_),
               &p.UFap_, sizeof(p.UFap_),
               &p.z_, sizeof(p.z_),
               &p.rho_, sizeof(p.rho_),
               &p.dt_, sizeof(p.dt_),
               &p.shift_, sizeof(p.shift_),
               &p.ghost_, sizeof(p.ghost_)
               );
#endif
        os  << static_cast<const Particle<mcParticle>&>(p);
        os.write
        (
            reinterpret_cast<const char*>(&p.m_),
            sizeof(p.m_) + sizeof(p.Updf_) +
            sizeof(p.UParticle_) + sizeof(p.UFap_) +
            sizeof(p.z_) + sizeof(p.rho_)  + sizeof(p.dt_) +
            sizeof(p.shift_) + sizeof(p.ghost_)
        );
    }

    // Check state of Ostream
    os.check("Ostream& operator<<(Ostream&, const mcParticle&)");

    return os;
}


// ************************************************************************* //